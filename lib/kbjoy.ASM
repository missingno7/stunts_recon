; INPUTDOC Module input_keyboard_joystick_services (owner input_keyboard_joystick_services); comment-only static
; INPUTDOC contract review.
; INPUTDOC Audited INPUTDOC contracts qualify the older retained generic comments; no code or declarations changed.
; INPUTDOC Routine names below follow rename-plan.json; definitions retain their original spelling.
; INPUTDOC dead=true means statically unreferenced; no proof against arbitrary external/numeric callers or execution.
; INPUTDOC Public data: _callbackflags2
; INPUTDOC Routine _set_bios_mode3 -> _set_bios_mode3: Clear the video surface, select text equipment bits, set BIOS
; INPUTDOC mode 3 and palette/background zero.
; INPUTDOC Routine _kb_parse_key -> _kb_parse_key: Dispatch a key callback or return its ASCII/extended game key word.
; INPUTDOC Routine _kb_reg_callback -> _kb_reg_callback: Reuse or allocate one of 64 far callback slots and assign its
; INPUTDOC one-based selector to a key.
; INPUTDOC Routine _nopsub_304AF -> _kb_clear_key_callback: Clear the callback selector for the supplied key by
; INPUTDOC entering the registration tail.
; INPUTDOC Routine _nopsub_304B6 -> _input_read_char_or_joystick: Read a pending BIOS key or return a delayed joystick
; INPUTDOC direction/button character.
; INPUTDOC Routine _kb_get_char -> _kb_get_char: Peek a key; consume and parse it only when SS equals DS.
; INPUTDOC Routine _get_kb_or_joy_flags -> _get_kb_or_joy_flags: Return configured keyboard control flags,
; falling back
; INPUTDOC to joystick only if all are zero.
; INPUTDOC Routine _nopsub_305C8 -> _input_get_action_buttons: OR configured keyboard action buttons with active-low
; INPUTDOC joystick buttons.
; INPUTDOC Routine _get_joy_flags -> _get_joy_flags: Measure gameport RC axes, update calibration and return
; INPUTDOC direction/button bits.
; INPUTDOC Routine _reset_joystick_selection -> _reset_joystick_selection: Enable joystick sampling and reset only the
; INPUTDOC X/Y calibration extrema.
; INPUTDOC Routine _joystick_flags_to_index -> _joystick_flags_to_index: Map low four direction bits through the
; INPUTDOC 16-entry direction-index table.
; INPUTDOC Routine _replay_axis_value -> _replay_axis_value: Scale the saved X-axis sample relative to its minimum and
; INPUTDOC center by 31.
; INPUTDOC Routine _nopsub_307FA -> _joystick_scale_saved_y: Scale the saved Y-axis sample relative to its minimum and
; INPUTDOC center by 31.
.8086
INCLUDE include\platform_hw.inc
name asm012_132056
DGROUP group _DATA
assume ds:DGROUP
_DATA segment word public 'DATA'
_callbackflags db 128 dup (0)
_callbackflags2 db 134 dup (0)
key_parser_busy_flag db 2 dup (0)
_callbacks db 256 dup (0)
; BIOS key words: scan code in AH, zero ASCII byte in AL; literal direction policy.
; Literal translation or hardware policy table; no analytic generator is inferred.
joystick_button_keycode_table dw 0000h, 4800h, 5000h, 0000h, 4D00h, 4900h, 5100h, 0000h
    dw 4B00h, 4700h, 4F00h, 0000h, 0000h, 0000h, 0000h, 0000h
joystick_repeat_previous_key dw 0
joystick_repeat_current_key dw 0
joystick_repeat_count db 2 dup (0)
; Literal translation or hardware policy table; no analytic generator is inferred.
_kbscancodes db 39h, 1Ch, 47h, 48h, 49h, 4Dh, 51h, 50h, 4Fh, 4Bh
_joyflag1 dw 0
_joyflag2 dw 0
_joybutton db 00h
_joyinput db 00h
joystick_x_minimum dw 00050h
joystick_x_sample_minimum dw 0
joystick_x_maximum dw 0
joystick_x_sample_maximum dw 0
joystick_x_calibration_ticks dw 00014h
joystick_x_left_threshold dw 0
joystick_x_right_threshold dw 0
joystick_y_minimum dw 00050h
joystick_y_sample_minimum dw 0
joystick_y_maximum dw 0
joystick_y_sample_maximum dw 0
joystick_y_calibration_ticks dw 00014h
joystick_y_up_threshold dw 0
joystick_y_down_threshold dw 0
joystick_x_scale_factor dw 0
joystick_y_scale_factor dw 0
; Literal translation or hardware policy table; no analytic generator is inferred.
joystick_flags_to_axis_table db 00h, 01h, 05h, 00h, 03h, 02h, 04h, 03h, 07h, 08h, 06h, 07h
    db 00h, 01h, 05h, 00h
extrn _kbinput:byte
extrn _joystick_enabled:byte
_DATA ends
extrn _clearscreen:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _callbackflags2
public _set_bios_mode3
public _interpretkey
public _interpretsub
public _interpretclear
public _input_read_char_or_joystick
public _normkeyi
public _joystate
public _joybuttons
public _get_joy_flags
public _reset_joystick_selection
public _convertjoy
public _replay_axis_value
public _joystick_scale_saved_y
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by set bios mode3.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, SP, ES; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): calls a BIOS video service. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _set_bios_mode3 (original _set_bios_mode3).
; INPUTDOC Purpose: Clear the video surface, select text equipment bits, set BIOS mode 3 and palette/background zero.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: No API return; ES=0040h before BIOS calls; BIOS results unspecified.
; INPUTDOC Preserves: No explicit saves; do not infer preserved registers across INT10. SP balanced.
; INPUTDOC Clobbers: Direct AX,BX,ES,FLAGS; video_clear_color additionally CX; BIOS INT10 transitive contract unknown.
; INPUTDOC Globals read: BDA 0040:0010 equipment word
; INPUTDOC Globals written: BDA 0040:0010 bits 4..5 := 01b; transitive video memory
; INPUTDOC Services: video_clear_color(0); INT10 AH=00 AL=03; INT10 AH=0B BH=00 BL=00
; INPUTDOC Reachability: Retained source callers or a live intra-module call; see report.
_set_bios_mode3 proc far
    xor     ax, ax
    push    ax
    call    far ptr _clearscreen
    add     sp, 2
    mov     ax, 40h
    mov     es, ax
    mov     ax, es:[10h]
    and     ax, 0FFCFh
    or      ax, 10h
    mov     es:[10h], ax
    mov     ah, 0
    mov     al, 3
    int     R5HW_BIOS_VIDEO_INT ;/* PLATFORM(video): calls a BIOS video service. */
    mov     ah, 0Bh
    mov     bx, 0
    int     R5HW_BIOS_VIDEO_INT ;/* PLATFORM(video): calls a BIOS video service. */
    retf
_set_bios_mode3 endp
; -----------------------------------------------------------------------------
; Purpose: Translate a BIOS key word into the game keyboard representation.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: no named stack
; arguments referenced.
; Register ABI: touches AX, BX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _callbackflags, _callbackflags2, key_parser_busy_flag.
; Globals written: key_parser_busy_flag.
; /* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_parse_key (original _kb_parse_key).
; INPUTDOC Purpose: Dispatch a key callback or return its ASCII/extended game key word.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: SS:BP+6 (entry SP+4): BIOS key u16; low byte nonzero selects ASCII, masked to 7 bits and
; overwritten
; INPUTDOC in caller stack slot; zero low byte selects high-byte index capped at 84h. DS=DGROUP.
; INPUTDOC Outputs: AX=normalized key when no callback; AX=0 after callback; busy early return leaves incoming AX
; INPUTDOC unchanged. IF=1.
; INPUTDOC Preserves: BP,SI,DI explicitly restored; callback must preserve DS for guard cleanup. SS:SP balanced on
; INPUTDOC normal callback return.
; INPUTDOC Clobbers: Direct AX,BX,FLAGS,IF; callback may transitively clobber CX,DX,DS,ES and other unsaved state.
; INPUTDOC Globals read: _callbackflags[0..127]; _callbackflags2[0..132]; _callbacks far-pointer table;
; INPUTDOC key_parser_busy_flag
; INPUTDOC Globals written: key_parser_busy_flag; caller key stack word for ASCII; callback-dependent globals
; INPUTDOC Services: CLI/STI; indirect FAR callback with no explicitly supplied argument words
; INPUTDOC Reachability: Live via kb_get_char; callback targets are explicitly registered by src/obj_seg031.c.
_interpretkey proc far
    push bp
    mov bp, sp
    cli ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
; INPUTDOC Busy return intentionally leaves AX untouched; STI does not restore entry IF.
    cmp byte ptr ds:[key_parser_busy_flag], 0
    je acquire_key_parser_lock
    sti ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
    pop bp
    retf
acquire_key_parser_lock:
    mov byte ptr ds:[key_parser_busy_flag], 1
    sti ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
    mov bx, [bp + 6]
    or bl, bl
    je select_keyboard_callback_table
    and bx, 7Fh
; INPUTDOC ASCII normalization rewrites the caller-owned argument word before any callback.
    mov [bp + 6], bx
    mov bl, _callbackflags[bx]
dispatch_key_callback:
    dec bx
    jl return_without_callback
    shl bx, 1
; INPUTDOC These pushes preserve SI/DI, not documented callback arguments; callback must preserve DS for guard
; cleanup.
    push di
    push si
    shl bx, 1
    call dword ptr _callbacks[bx]
    pop si
    pop di
    xor ax, ax
    mov byte ptr ds:[key_parser_busy_flag], 0
    pop bp
    retf
select_keyboard_callback_table:
    mov bl, bh
    xor bh, bh
; INPUTDOC 84h is a valid last index in the 134-byte scan selector allocation.
    cmp bx, 84h
    jl dispatch_keyboard_callback
    mov bx, 84h
dispatch_keyboard_callback:
    mov bl, _callbackflags2[bx]
    jmp dispatch_key_callback
return_without_callback:
    mov ax, [bp + 6]
    mov byte ptr ds:[key_parser_busy_flag], 0
    pop bp
    retf
_interpretkey endp
; -----------------------------------------------------------------------------
; Purpose: Register a keyboard input callback.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6);
; arg_2 (word ptr 8); arg_4 (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, BP, SP; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _callbackflags, _callbackflags2.
; Global addresses taken: _callbacks.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_reg_callback (original _kb_reg_callback).
; INPUTDOC Purpose: Reuse or allocate one of 64 far callback slots and assign its one-based selector to a key.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: SS:BP+6 (entry SP+4): encoded key; +8 (entry +6): callback offset; +10 (entry +8): callback
; segment.
; INPUTDOC DS=DGROUP. ASCII branch expects key 1..127; signed JG does not reject negative full words.
; INPUTDOC Outputs: No specified return; AX=one-based slot selector on found/free path, original pointer offset on
; INPUTDOC full-table failure.
; INPUTDOC Preserves: Untouched by this body: BP, DI, DS, ES, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX,BX,CX,DX,FLAGS
; INPUTDOC Globals read: _callbacks[64] offset/segment slots
; INPUTDOC Globals written: _callbacks when free segment=0; _callbackflags or _callbackflags2
; INPUTDOC Services:
; INPUTDOC Reachability: Live: lib/startup.c:161-177 registers nine key bindings.
_interpretsub proc far
    arg_encoded_key = word ptr 6
    arg_callback_offset = word ptr 8
    arg_callback_segment = word ptr 10

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_callback_offset]
    mov     cx, 40h ; '@'
    mov     bx, offset DGROUP:_callbacks
    mov     dx, [bp+arg_callback_segment]
scan_callback_slots:
    cmp     [bx], ax
    jnz     short check_free_callback_slot
    cmp     [bx+2], dx
    jz      short calculate_callback_flag
check_free_callback_slot:
; INPUTDOC A zero callback segment marks a free slot; an identical pointer reuses the slot first.
    cmp     word ptr [bx+2], 0
    jz      short store_callback_entry
    add     bx, 4
    loop    scan_callback_slots
    pop     bp
    retf
store_callback_entry:
    mov     [bx], ax
    mov     [bx+2], dx
calculate_callback_flag:
    mov     ax, 41h ; 'A'
    sub     ax, cx
store_callback_flags:
    mov     bx, [bp+arg_encoded_key]
    or      bl, bl
    jz      short store_scancode_flags
; INPUTDOC Signed JG rejects positive keys above 127, but negative encoded words pass through.
    cmp     bx, 7Fh ; ''
    jg      short return_after_keyboard_flag
    mov     _callbackflags[bx], al
return_after_keyboard_flag:
    pop     bp
    retf
store_scancode_flags:
    mov     bl, bh
    xor     bh, bh
    cmp     bx, 84h ; ''
    jg      short return_after_keyboard_flag
    mov     _callbackflags2[bx], al
    pop     bp
    retf

_interpretsub endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 304AF.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; no named stack arguments referenced.
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_clear_key_callback (original _nopsub_304AF).
; INPUTDOC Purpose: Clear the callback selector for the supplied key by entering the registration tail.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: SS:BP+6 (entry SP+4): encoded key, same index rules as kb_reg_callback. DS=DGROUP.
; INPUTDOC Outputs: AX=0; callback pointer slots remain allocated.
; INPUTDOC Preserves: Untouched by this body: BP, CX, DI, DS, DX, ES, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX,BX,FLAGS
; INPUTDOC Globals read:
; INPUTDOC Globals written: _callbackflags or _callbackflags2
; INPUTDOC Services:
; INPUTDOC Reachability: Statically unreferenced; no proof against arbitrary external/numeric callers. No reference
; INPUTDOC outside its definition/declaration, installed vector or stored pointer was found in active asm/ and src/.
; INPUTDOC dead=true uses this bounded static convention, not observed execution.
_interpretclear proc far
    push    bp
    mov     bp, sp
    xor     ax, ax
; INPUTDOC Shared tail pops this routine BP and RETFs using its own caller frame.
    jmp     short store_callback_flags

_interpretclear endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 304B6. Its internal flow uses return_empty_key_char,
; poll_joystick_for_character.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: no named stack
; arguments referenced.
; Register ABI: touches AX, BX, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: joystick_button_keycode_table, joystick_repeat_count, joystick_repeat_current_key,
; joystick_repeat_previous_key.
; Globals written: joystick_repeat_count, joystick_repeat_current_key, joystick_repeat_previous_key.
; /* PLATFORM(input_kb): calls the BIOS keyboard service. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _input_read_char_or_joystick (original _nopsub_304B6).
; INPUTDOC Purpose: Read a pending BIOS key or return a delayed joystick direction/button character.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: AX=ASCII with AH=0, extended key word, joystick-mapped key or zero while unchanged/delayed.
; INPUTDOC Preserves: Untouched by this body: BP, DI, DS, ES, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX,BX,CX,DX,FLAGS,IF through get_joy_flags; INT16 external contract unknown.
; INPUTDOC Globals read: joystick_button_keycode_table; joystick_repeat_previous_key; joystick_repeat_current_key;
; INPUTDOC joystick_repeat_count; transitive get_joy_flags globals
; INPUTDOC Globals written: joystick_repeat_previous_key; joystick_repeat_current_key; joystick_repeat_count;
; INPUTDOC transitive get_joy_flags globals
; INPUTDOC Services: INT16 AH=01/00; get_joy_flags
; INPUTDOC Reachability: Statically unreferenced; no proof against arbitrary external/numeric callers. No reference
; INPUTDOC outside its definition/declaration, installed vector or stored pointer was found in active asm/ and src/.
; INPUTDOC dead=true uses this bounded static convention, not observed execution.
_input_read_char_or_joystick proc far
    mov     ah, 1
    int     R5HW_BIOS_KEYBOARD_INT
; KEYBOARD - CHECK BUFFER, DO NOT CLEAR /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jz      short poll_joystick_for_character
    mov     ah, 0
    int     R5HW_BIOS_KEYBOARD_INT
; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    or      al, al
    jz      short return_empty_key_char
    xor     ah, ah
return_empty_key_char:
    retf
; INPUTDOC No preceding fall-through: this second BIOS/parser body follows RETF and has no local incoming edge.
    mov     ah, 1
    int     R5HW_BIOS_KEYBOARD_INT
; KEYBOARD - CHECK BUFFER, DO NOT CLEAR /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jz      short poll_joystick_for_character
    mov     ah, 0
    int     R5HW_BIOS_KEYBOARD_INT
; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    push    ax
    call far ptr _interpretkey
    add     sp, 2
    retf
poll_joystick_for_character:
    call far ptr _get_joy_flags
    mov     bx, ax
    test    ax, 30h
    jz      short map_joystick_button_char
    mov     ax, 0Dh
    jmp     short compare_joystick_repeat_key
    nop ; retained code pad at an even offset; EVEN would emit no byte
map_joystick_button_char:

    and     bx, 0Fh

    shl     bx, 1
    mov     ax, joystick_button_keycode_table[bx]
compare_joystick_repeat_key:
    cmp     ax, joystick_repeat_previous_key
    jnz     short check_repeat_key_delay
return_when_key_unchanged:
    xor     ax, ax
    retf
check_repeat_key_delay:
    cmp     ax, joystick_repeat_current_key
    jnz     short start_key_repeat_delay
; INPUTDOC Delay is BYTE-sized because joystick_repeat_count is declared DB; second allocated byte is untouched.
    dec     joystick_repeat_count
    jg      short return_when_key_unchanged
    mov     joystick_repeat_previous_key, ax
    retf
start_key_repeat_delay:
    mov     joystick_repeat_count, 3
    mov     joystick_repeat_current_key, ax
    jmp     short return_when_key_unchanged

_input_read_char_or_joystick endp
; -----------------------------------------------------------------------------
; Purpose: Read a character from the BIOS keyboard buffer.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: no named stack
; arguments referenced.
; Register ABI: touches AX, BX, CX, SP, DS, SS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(input_kb): calls the BIOS keyboard service. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_get_char (original _kb_get_char).
; INPUTDOC Purpose: Peek a key; consume and parse it only when SS equals DS.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP, SS compared with DS to select consume path.
; INPUTDOC Outputs: AX=0 if absent; if SS!=DS, raw peeked AX without dequeue; otherwise kb_parse_key result (callback
; INPUTDOC may consume).
; INPUTDOC Preserves: BP,SI,DI,SS under parser callback DS-preserving requirement; SP balanced. Other state depends on
; INPUTDOC callback/BIOS.
; INPUTDOC Clobbers: Direct AX,BX,CX,FLAGS; parser/callback transitive clobbers conditional; INT16 external contract
; INPUTDOC unknown.
; INPUTDOC Globals read: transitive keyboard queue and parser tables/guard
; INPUTDOC Globals written: transitive queue dequeue, parser guard, callback state
; INPUTDOC Services: INT16 AH=01/00; kb_parse_key
; INPUTDOC Reachability: Retained source callers or a live intra-module call; see report.
_normkeyi proc far
    mov     ah, 1
    int     R5HW_BIOS_KEYBOARD_INT
; KEYBOARD - CHECK BUFFER, DO NOT CLEAR /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jnz     short read_buffered_key_char
    xor     ax, ax
    retf
read_buffered_key_char:
; INPUTDOC Different DS/SS means peek only; no dequeue and no callback parsing.
    mov     bx, ss
    mov     cx, ds
    cmp     bx, cx
    jnz     short return_keyboard_character
    mov     ah, 0
    int     R5HW_BIOS_KEYBOARD_INT
; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    push    ax
    call far ptr _interpretkey
    add     sp, 2
return_keyboard_character:
    retf

_normkeyi endp
; -----------------------------------------------------------------------------
; Purpose: Return the combined keyboard or joystick input flags.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: no named stack
; arguments referenced.
; Register ABI: touches AX, BX; balanced explicit saves/restores observed: none inferred; path-specific inputs
; and outputs follow the listed operands.
; Globals read: _kbinput, _kbscancodes.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _get_kb_or_joy_flags (original _get_kb_or_joy_flags).
; INPUTDOC Purpose: Return configured keyboard control flags, falling back to joystick only if all are zero.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: AX=bits 01 up,02 down,04 right,08 left,10 first action,20 second action; AH=0.
; INPUTDOC Preserves: Untouched by this body: BP, DI, DS, ES, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX,BX,FLAGS; CX,DX,IF additionally on enabled joystick fallback.
; INPUTDOC Globals read: _kbinput; _kbscancodes[10]; transitive get_joy_flags globals
; INPUTDOC Globals written: transitive get_joy_flags globals on fallback
; INPUTDOC Services: get_joy_flags only when keyboard flags are zero
; INPUTDOC Reachability: Retained source callers or a live intra-module call; see report.
_joystate proc far
    xor     ax, ax
    xor     bx, bx
    mov     bl, _kbscancodes
    cmp     _kbinput[bx], 0
    jz      short check_enter_key
    or      al, 10h
check_enter_key:
    mov     bl, _kbscancodes+1
    cmp     _kbinput[bx], 0
    jz      short check_up_left_key
    or      al, 20h
check_up_left_key:
    mov     bl, _kbscancodes+2
    cmp     _kbinput[bx], 0
    jz      short check_up_key
    or      al, 09h
check_up_key:
    mov     bl, _kbscancodes+3
    cmp     _kbinput[bx], 0
    jz      short check_up_right_key
    or      al, 01h
check_up_right_key:
    mov     bl, _kbscancodes+4
    cmp     _kbinput[bx], 0
    jz      short check_right_key
    or      al, 05h
check_right_key:
    mov     bl, _kbscancodes+5
    cmp     _kbinput[bx], 0
    jz      short check_down_right_key
    or      al, 04h
check_down_right_key:
    mov     bl, _kbscancodes+6
    cmp     _kbinput[bx], 0
    jz      short check_down_key
    or      al, 06h
check_down_key:
    mov     bl, _kbscancodes+7
    cmp     _kbinput[bx], 0
    jz      short check_down_left_key
    or      al, 02h
check_down_left_key:
    mov     bl, _kbscancodes+8
    cmp     _kbinput[bx], 0
    jz      short check_left_key
    or      al, 0Ah
check_left_key:
    mov     bl, _kbscancodes+9
    cmp     _kbinput[bx], 0
    jz      short use_joystick_when_no_key
    or      al, 08h
use_joystick_when_no_key:
    or      ax, ax
    jnz     short return_input_flags
; INPUTDOC Keyboard has priority; joystick bits are not ORed into a nonzero keyboard result.
    call far ptr _get_joy_flags
return_input_flags:
    retf

_joystate endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 305C8. Its internal flow uses
; check_second_keyboard_scancode, read_joystick_buttons.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: no named stack
; arguments referenced.
; Register ABI: touches AX, BX, CX, DX; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _joystick_enabled, _kbinput, _kbscancodes.
; Globals written: none identified.
; /* PLATFORM(input_joy): accesses the joystick game port. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _input_get_action_buttons (original _nopsub_305C8).
; INPUTDOC Purpose: OR configured keyboard action buttons with active-low joystick buttons.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: AX contains only 10h/20h button flags; no direction sampling.
; INPUTDOC Preserves: Untouched by this body: BP, DI, DS, ES, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX,BX,CX,DX,FLAGS
; INPUTDOC Globals read: _kbinput; _kbscancodes[0..1]; _joystick_enabled bit 0
; INPUTDOC Globals written:
; INPUTDOC Services: IN 201h if enabled
; INPUTDOC Reachability: Statically unreferenced; no proof against arbitrary external/numeric callers. No reference
; INPUTDOC outside its definition/declaration, installed vector or stored pointer was found in active asm/ and src/.
; INPUTDOC dead=true uses this bounded static convention, not observed execution.
_joybuttons proc far
    xor     cx, cx
    xor     bx, bx
    mov     bl, _kbscancodes
    cmp     byte ptr _kbinput[bx], 0
    jz      short check_second_keyboard_scancode
    or      cl, 10h
check_second_keyboard_scancode:
    mov     bl, _kbscancodes+1
    cmp     byte ptr _kbinput[bx], 0
    jz      short read_joystick_buttons
    or      cl, 20h
read_joystick_buttons:
    xor     ax, ax
    mov     dx, R5HW_JOYSTICK_GAME_PORT
    test    _joystick_enabled, 01h
    jz      short return_combined_input_flags
    in      al, dx ; Game I/O port /* PLATFORM(input_joy): accesses the joystick game port. */
    not     al

    and     al, 30h

return_combined_input_flags:
    or      ax, cx
    retf

_joybuttons endp
; -----------------------------------------------------------------------------
; Purpose: Read and normalize the game-port joystick button and axis state.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, CX, DX; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _joystick_enabled, _joybutton, _joyflag1, _joyflag2, _joyinput, joystick_x_calibration_ticks,
; joystick_x_left_threshold, joystick_x_maximum, joystick_x_minimum,
; joystick_x_right_threshold, joystick_x_sample_maximum, joystick_x_sample_minimum,
; joystick_y_calibration_ticks, joystick_y_down_threshold, joystick_y_maximum,
;   joystick_y_minimum, joystick_y_sample_maximum, joystick_y_sample_minimum, joystick_y_up_threshold.
; Globals written: _joybutton, _joyflag1, _joyflag2, _joyinput, joystick_x_calibration_ticks,
; joystick_x_left_threshold, joystick_x_maximum, joystick_x_minimum,
; joystick_x_right_threshold, joystick_x_sample_maximum, joystick_x_sample_minimum, joystick_x_scale_factor,
; joystick_y_calibration_ticks, joystick_y_down_threshold,
; joystick_y_maximum, joystick_y_minimum, joystick_y_sample_maximum, joystick_y_sample_minimum,
; joystick_y_scale_factor, joystick_y_up_threshold.
; /* PLATFORM(input_joy): accesses the joystick game port. */
; /* PLATFORM(bios): changes the processor interrupt-enable state. */
; -----------------------------------------------------------------------------
; INPUTDOC Contract _get_joy_flags (original _get_joy_flags).
; INPUTDOC Purpose: Measure gameport RC axes, update calibration and return direction/button bits.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: AX=0 without sampling when disabled; else zero-extended flags 01/02/04/08/10/20. Enabled path
; INPUTDOC returns IF=1.
; INPUTDOC Preserves: Untouched by this body: BP, DI, DS, ES, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX,BX,CX,DX,FLAGS,IF; disabled path only AX and arithmetic FLAGS.
; INPUTDOC Globals read: _joystick_enabled; _joybutton; _joyflag1; _joyflag2; _joyinput; joystick_x_minimum;
; INPUTDOC joystick_x_sample_minimum; joystick_x_maximum; joystick_x_sample_maximum; joystick_x_calibration_ticks;
; INPUTDOC joystick_x_left_threshold; joystick_x_right_threshold; joystick_y_minimum; joystick_y_sample_minimum;
; INPUTDOC joystick_y_maximum; joystick_y_sample_maximum; joystick_y_calibration_ticks; joystick_y_up_threshold;
; INPUTDOC joystick_y_down_threshold
; INPUTDOC Globals written: _joybutton; _joyflag1; _joyflag2; _joyinput; joystick_x_minimum;
; joystick_x_sample_minimum;
; INPUTDOC joystick_x_maximum; joystick_x_sample_maximum; joystick_x_calibration_ticks; joystick_x_left_threshold;
; INPUTDOC joystick_x_right_threshold; joystick_x_scale_factor; joystick_y_minimum; joystick_y_sample_minimum;
; INPUTDOC joystick_y_maximum; joystick_y_sample_maximum; joystick_y_calibration_ticks; joystick_y_up_threshold;
; INPUTDOC joystick_y_down_threshold; joystick_y_scale_factor
; INPUTDOC Services: IN/OUT 201h; CLI/STI around axis timing
; INPUTDOC Reachability: Retained source callers or a live intra-module call; see report.
_get_joy_flags proc far
    test    _joystick_enabled, 01h
    jnz     short start_joystick_sampling
    xor     ax, ax
    retf
start_joystick_sampling:
    mov     _joyinput, 0
    mov     dx, R5HW_JOYSTICK_GAME_PORT
    in      al, dx ; Game I/O port /* PLATFORM(input_joy): accesses the joystick game port. */
    mov     _joybutton, al
    mov     bl, 3
    mov     _joyflag1, 50h ; 'P'
    mov     _joyflag2, 50h ; 'P'
; INPUTDOC Interrupts are unconditionally reenabled at calibrate_x_axis after bounded RC polling.
    cli ;/* PLATFORM(bios): changes the processor interrupt-enable state. */
    out     dx, al ; Game I/O port /* PLATFORM(input_joy): accesses the joystick game port. */
    mov     cx, 14h
joystick_sampling_delay:
    loop    joystick_sampling_delay
    mov     cx, 0
sample_joystick_axes:
    in      al, dx ; Game I/O port /* PLATFORM(input_joy): accesses the joystick game port. */
    and     al, bl
    xor     al, bl
    jnz     short classify_axis_sample
    inc     cx
    cmp     cx, 0FA0h
    jl      short sample_joystick_axes
    jmp     short calibrate_x_axis
    nop ; retained code pad at an even offset; EVEN would emit no byte
classify_axis_sample:
    test    al, 01h
    jnz     short store_x_axis_sample
store_y_axis_sample:
    test    al, 02h
    jz      short sample_joystick_axes
    mov     _joyflag2, cx

    and     bl, 01h

    jnz     short sample_joystick_axes
    jmp     short calibrate_x_axis
    nop ; retained code pad at an even offset; EVEN would emit no byte
store_x_axis_sample:
    mov     _joyflag1, cx

    and     bl, 02h

    jnz     short store_y_axis_sample
calibrate_x_axis:
    sti ;/* PLATFORM(bios): changes the processor interrupt-enable state. */
    mov     ax, _joyflag1
    cmp     ax, joystick_x_minimum
    jge     short update_x_axis_maximum
; INPUTDOC Calibration tick countdown and extrema use the exact signed branches shown; Y differs.
    dec     joystick_x_calibration_ticks
    jle     short set_x_calibration_minimum
    cmp     ax, joystick_x_sample_minimum
    jl      short classify_x_axis_direction
    mov     joystick_x_sample_minimum, ax
    jmp     short classify_x_axis_direction
    even ; word-align the following entry
set_x_calibration_minimum:
    mov     bx, joystick_x_sample_minimum
    mov     joystick_x_minimum, bx
    mov     bx, joystick_x_maximum
compute_x_axis_span:
    sub     bx, joystick_x_minimum
    jle     short compute_x_axis_thresholds
    xor     dx, dx
    mov     ax, 4000h
    div     bx
    mov     joystick_x_scale_factor, ax
compute_x_axis_thresholds:
    mov     ax, bx
    mov     dx, R5HW_JOYSTICK_GAME_PORT
    shr     ax, 1
    mov     bx, ax
    shr     bx, 1
    add     ax, joystick_x_minimum
    add     ax, bx
    mov     joystick_x_right_threshold, ax
    sub     ax, bx
    sub     ax, bx
    mov     joystick_x_left_threshold, ax
    mov     ax, _joyflag1
    jmp     short reset_x_axis_calibration
    nop ; retained code pad at an even offset; EVEN would emit no byte
update_x_axis_maximum:
    cmp     ax, joystick_x_maximum
    jle     short reset_x_axis_calibration
    dec     joystick_x_calibration_ticks
    jle     short finish_x_axis_calibration
    cmp     ax, joystick_x_sample_maximum
    jge     short classify_x_axis_direction
    mov     joystick_x_sample_maximum, ax
    jmp     short classify_x_axis_direction
    nop ; retained code pad at an even offset; EVEN would emit no byte
finish_x_axis_calibration:
    mov     bx, joystick_x_sample_maximum
    mov     joystick_x_maximum, bx
    jmp     short compute_x_axis_span
reset_x_axis_calibration:
    mov     joystick_x_calibration_ticks, 14h
    mov     joystick_x_sample_maximum, 4E20h
    mov     joystick_x_sample_minimum, 0
classify_x_axis_direction:
    cmp     ax, joystick_x_left_threshold
    jl      short set_left_direction_flag
    cmp     ax, joystick_x_right_threshold
    jl      short calibrate_y_axis
    or      _joyinput, 04h
calibrate_y_axis:
    mov     ax, _joyflag2
    cmp     ax, joystick_y_minimum
; INPUTDOC Y minimum comparison is unsigned JNB; its maximum countdown later uses JZ, unlike X JLE.
    jnb     short update_y_axis_maximum
    dec     joystick_y_calibration_ticks
    jle     short set_y_calibration_minimum
    cmp     ax, joystick_y_sample_minimum
    jl      short classify_y_axis_direction
    mov     joystick_y_sample_minimum, ax
    jmp     short classify_y_axis_direction
    nop ; retained code pad at an even offset; EVEN would emit no byte
set_y_calibration_minimum:
    mov     bx, joystick_y_sample_minimum
    mov     joystick_y_minimum, bx
    mov     bx, joystick_y_maximum
compute_y_axis_span:
    sub     bx, joystick_y_minimum
    jle     short compute_y_axis_thresholds
    xor     dx, dx
    mov     ax, 4000h
    div     bx
    mov     joystick_y_scale_factor, ax
compute_y_axis_thresholds:
    mov     ax, bx
    mov     dx, R5HW_JOYSTICK_GAME_PORT
    shr     ax, 1
    mov     bx, ax
    shr     bx, 1
    add     ax, joystick_y_minimum
    add     ax, bx
    mov     joystick_y_down_threshold, ax
    sub     ax, bx
    sub     ax, bx
    mov     joystick_y_up_threshold, ax
    mov     ax, _joyflag2
    jmp     short reset_y_axis_calibration
    even ; word-align the following entry
set_left_direction_flag:
    or      _joyinput, 08h
    jmp     short calibrate_y_axis
update_y_axis_maximum:
    cmp     ax, joystick_y_maximum
    jle     short reset_y_axis_calibration
    dec     joystick_y_calibration_ticks
    jz      short finish_y_axis_calibration
store_new_y_axis_sample:
    cmp     ax, joystick_y_sample_maximum
    jge     short classify_y_axis_direction
    mov     joystick_y_sample_maximum, ax
    jmp     short classify_y_axis_direction
    nop ; retained code pad at an even offset; EVEN would emit no byte
finish_y_axis_calibration:
    mov     bx, joystick_y_sample_maximum
    mov     joystick_y_maximum, bx
    jmp     short compute_y_axis_span
reset_y_axis_calibration:
    mov     joystick_y_calibration_ticks, 14h
    mov     joystick_y_sample_maximum, 4E20h
    mov     joystick_y_sample_minimum, 0
classify_y_axis_direction:
    cmp     ax, joystick_y_up_threshold
    jb      short set_up_direction_flag
    cmp     ax, joystick_y_down_threshold
    jb      short read_joystick_button_flags
    or      _joyinput, 02h
read_joystick_button_flags:
; INPUTDOC Button is logically pressed if either first or last active-low sample was low.
    in      al, dx ; Game I/O port /* PLATFORM(input_joy): accesses the joystick game port. */
    and     al, _joybutton

    and     al, 30h

    xor     al, 30h
    or      _joyinput, al
    mov     al, _joyinput
    xor     ah, ah
    retf
set_up_direction_flag:
    or      _joyinput, 01h
    jmp     short read_joystick_button_flags

_get_joy_flags endp
; -----------------------------------------------------------------------------
; Purpose: Reset the recorded joystick selection state.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _joystick_enabled, joystick_x_maximum, joystick_x_minimum, joystick_y_maximum, joystick_y_minimum.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _reset_joystick_selection (original _reset_joystick_selection).
; INPUTDOC Purpose: Enable joystick sampling and reset only the X/Y calibration extrema.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: No specified return; AX and all other registers untouched.
; INPUTDOC Preserves: All registers and FLAGS; SP balanced.
; INPUTDOC Clobbers: None (MOV stores do not change FLAGS).
; INPUTDOC Globals read:
; INPUTDOC Globals written: _joystick_enabled :=1; joystick_x_minimum :=0050h; joystick_x_maximum :=0;
; INPUTDOC joystick_y_minimum :=0050h; joystick_y_maximum :=0
; INPUTDOC Services:
; INPUTDOC Reachability: Retained source callers or a live intra-module call; see report.
_reset_joystick_selection proc far
; INPUTDOC Only enable and extrema reset; prior thresholds, scale factors and countdowns survive.
    mov     _joystick_enabled, 1
    mov     joystick_x_minimum, 50h
    mov     joystick_x_maximum, 0
    mov     joystick_y_minimum, 50h
    mov     joystick_y_maximum, 0
    retf
_reset_joystick_selection endp
; -----------------------------------------------------------------------------
; Purpose: Convert joystick state flags to the selected joystick index.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches BX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: joystick_flags_to_axis_table.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _joystick_flags_to_index (original _joystick_flags_to_index).
; INPUTDOC Purpose: Map low four direction bits through the 16-entry direction-index table.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: SS:BP+6 (entry SP+4): u16 flags; only bits 0..3 used. DS=DGROUP.
; INPUTDOC Outputs: AX=zero-extended table byte (0..8).
; INPUTDOC Preserves: Untouched by this body: BP, CX, DI, DS, DX, ES, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX,BX,FLAGS
; INPUTDOC Globals read: joystick_flags_to_axis_table[16]
; INPUTDOC Globals written:
; INPUTDOC Services:
; INPUTDOC Reachability: Retained source callers or a live intra-module call; see report.
_convertjoy proc far
    arg_axis_flags = word ptr 6

    push    bp
    mov     bp, sp
    mov     bx, [bp+arg_axis_flags]

    and     bx, 0Fh

    mov     al, joystick_flags_to_axis_table[bx]
    xor     ah, ah
    pop     bp
    retf
_convertjoy endp
; -----------------------------------------------------------------------------
; Purpose: Return a recorded joystick axis value for replay.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: _joyflag1, joystick_x_minimum, joystick_x_scale_factor.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _replay_axis_value (original _replay_axis_value).
; INPUTDOC Purpose: Scale the saved X-axis sample relative to its minimum and center by 31.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: Let d=(sample-min) mod 65536, replaced with 0 iff signed16(sample)<signed16(min).
; AX=((u32(d)*scale
; INPUTDOC >>8) & FFFFh)-31 modulo 65536; DX retains product high word. Retained C caller consumes AL as signed byte.
; INPUTDOC Preserves: Untouched by this body: BP, BX, CX, DI, DS, ES, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX,DX,FLAGS
; INPUTDOC Globals read: _joyflag1; joystick_x_minimum; joystick_x_scale_factor
; INPUTDOC Globals written:
; INPUTDOC Services:
; INPUTDOC Reachability: Retained source callers or a live intra-module call; see report.
_replay_axis_value proc far

    mov     ax, _joyflag1
    sub     ax, joystick_x_minimum
    jge     short clamp_replayed_x_axis
    xor     ax, ax
clamp_replayed_x_axis:
; INPUTDOC MUL is unsigned after signed subtraction clamp; byte shuffling extracts product bits 8..23.
    mul     joystick_x_scale_factor
    mov     al, ah
    mov     ah, dl
    sub     ax, 1Fh
    retf
_replay_axis_value endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 307FA. Its internal flow uses scale_replayed_y_axis.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: _joyflag2, joystick_y_minimum, joystick_y_scale_factor.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _joystick_scale_saved_y (original _nopsub_307FA).
; INPUTDOC Purpose: Scale the saved Y-axis sample relative to its minimum and center by 31.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: Let d=(sample-min) mod 65536, replaced with 0 iff signed16(sample)<signed16(min).
; AX=((u32(d)*scale
; INPUTDOC >>8) & FFFFh)-31 modulo 65536; DX retains product high word.
; INPUTDOC Preserves: Untouched by this body: BP, BX, CX, DI, DS, ES, SI, SS; SP balanced.
; INPUTDOC Clobbers: AX,DX,FLAGS
; INPUTDOC Globals read: _joyflag2; joystick_y_minimum; joystick_y_scale_factor
; INPUTDOC Globals written:
; INPUTDOC Services:
; INPUTDOC Reachability: Statically unreferenced; no proof against arbitrary external/numeric callers. No reference
; INPUTDOC outside its definition/declaration, installed vector or stored pointer was found in active asm/ and src/.
; INPUTDOC dead=true uses this bounded static convention, not observed execution.
_joystick_scale_saved_y proc far
    mov     ax, _joyflag2
    sub     ax, joystick_y_minimum
    jge     short scale_replayed_y_axis
    xor     ax, ax
scale_replayed_y_axis:
    mul     joystick_y_scale_factor
    mov     al, ah
    mov     ah, dl
    sub     ax, 1Fh
    retf
    db 0 ; retained zero fill at this boundary
_joystick_scale_saved_y endp
S012_TEXT ends
end
