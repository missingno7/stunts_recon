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
joystick_button_keycode_table dw 0, 04800h, 05000h, 0, 04D00h, 04900h, 05100h, 0
    dw 04B00h, 04700h, 04F00h, 0, 0, 0, 0, 0
joystick_repeat_previous_key dw 0
joystick_repeat_current_key dw 0
joystick_repeat_count db 2 dup (0)
_kbscancodes db 039h, 01Ch, 047h, 048h, 049h, 04Dh, 051h, 050h, 04Fh, 04Bh
_joyflag1 dw 0
_joyflag2 dw 0
_joybutton db 0
_joyinput db 0
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
joystick_flags_to_axis_table db 0, 1, 5, 0, 3, 2, 4, 3, 7, 8, 6, 7, 0, 1, 5, 0
extrn _kbinput:byte
extrn _byte_3FE00:byte
_DATA ends
assume ds:DGROUP
extrn _video_clear_color:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _callbackflags2
public _set_bios_mode3
public _kb_parse_key
public _kb_reg_callback
public _nopsub_304AF
public _nopsub_304B6
public _kb_get_char
public _get_kb_or_joy_flags
public _nopsub_305C8
public _get_joy_flags
public _reset_joystick_selection
public _joystick_flags_to_index
public _replay_axis_value
public _nopsub_307FA
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by set bios mode3.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, SP, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): calls a BIOS video service. */
; -----------------------------------------------------------------------------
_set_bios_mode3 proc far
    xor     ax, ax
    push    ax
    call    far ptr _video_clear_color
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
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: _callbackflags, _callbackflags2, key_parser_busy_flag.
; Globals written: key_parser_busy_flag.
; /* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
; -----------------------------------------------------------------------------
_kb_parse_key proc far
    push bp
    mov bp, sp
    cli ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
    cmp byte ptr ds:[key_parser_busy_flag], 0
    je acquire_key_parser_lock
    sti ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
    pop bp
    retf
acquire_key_parser_lock:
    mov byte ptr ds:[key_parser_busy_flag], 1
    sti ;/* PLATFORM(input_kb): masks or restores interrupts around keyboard state. */
    mov bx, word ptr [bp + 6]
    or bl, bl
    je select_keyboard_callback_table
    and bx, 7Fh
    mov word ptr [bp + 6], bx
    mov bl, byte ptr _callbackflags[bx]
dispatch_key_callback:
    dec bx
    jl return_without_callback
    shl bx, 1
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
    cmp bx, 84h
    jl dispatch_keyboard_callback
    mov bx, 84h
dispatch_keyboard_callback:
    mov bl, byte ptr _callbackflags2[bx]
    jmp dispatch_key_callback
return_without_callback:
    mov ax, word ptr [bp + 6]
    mov byte ptr ds:[key_parser_busy_flag], 0
    pop bp
    retf
_kb_parse_key endp
; -----------------------------------------------------------------------------
; Purpose: Register a keyboard input callback.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _callbackflags, _callbackflags2.
; Global addresses taken: _callbacks.
; -----------------------------------------------------------------------------
_kb_reg_callback proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_2]
    mov     cx, 40h ; '@'
    mov     bx, offset DGROUP:_callbacks
    mov     dx, [bp+arg_4]
scan_callback_slots:
    cmp     [bx], ax
    jnz     short check_free_callback_slot
    cmp     [bx+2], dx
    jz      short calculate_callback_flag
check_free_callback_slot:
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
    mov     bx, [bp+arg_0]
    or      bl, bl
    jz      short store_scancode_flags
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

_kb_reg_callback endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 304AF.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_304AF proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    xor     ax, ax
    jmp     short store_callback_flags

_nopsub_304AF endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 304B6. Its internal flow uses return_empty_key_char, poll_joystick_for_character.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: joystick_button_keycode_table, joystick_repeat_count, joystick_repeat_current_key, joystick_repeat_previous_key.
; Globals written: joystick_repeat_count, joystick_repeat_current_key, joystick_repeat_previous_key.
; /* PLATFORM(input_kb): calls the BIOS keyboard service. */
; -----------------------------------------------------------------------------
_nopsub_304B6 proc far
    mov     ah, 1
    int     R5HW_BIOS_KEYBOARD_INT ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jz      short poll_joystick_for_character
    mov     ah, 0
    int     R5HW_BIOS_KEYBOARD_INT ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    or      al, al
    jz      short return_empty_key_char
    xor     ah, ah
return_empty_key_char:
    retf
    mov     ah, 1
    int     R5HW_BIOS_KEYBOARD_INT ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jz      short poll_joystick_for_character
    mov     ah, 0
    int     R5HW_BIOS_KEYBOARD_INT ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    push    ax
    call far ptr _kb_parse_key
    add     sp, 2
    retf
poll_joystick_for_character:
    call far ptr _get_joy_flags
    mov     bx, ax
    test    ax, 30h
    jz      short map_joystick_button_char
    mov     ax, 0Dh
    jmp     short compare_joystick_repeat_key
    db 144
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
    dec     joystick_repeat_count
    jg      short return_when_key_unchanged
    mov     joystick_repeat_previous_key, ax
    retf
start_key_repeat_delay:
    mov     joystick_repeat_count, 3
    mov     joystick_repeat_current_key, ax
    jmp     short return_when_key_unchanged

_nopsub_304B6 endp
; -----------------------------------------------------------------------------
; Purpose: Read a character from the BIOS keyboard buffer.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, SP, DS, SS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(input_kb): calls the BIOS keyboard service. */
; -----------------------------------------------------------------------------
_kb_get_char proc far
    mov     ah, 1
    int     R5HW_BIOS_KEYBOARD_INT ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    jnz     short read_buffered_key_char
    xor     ax, ax
    retf
read_buffered_key_char:
    mov     bx, ss
    mov     cx, ds
    cmp     bx, cx
    jnz     short return_keyboard_character
    mov     ah, 0
    int     R5HW_BIOS_KEYBOARD_INT ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY /* PLATFORM(input_kb): calls the BIOS keyboard service. */
    push    ax
    call far ptr _kb_parse_key
    add     sp, 2
return_keyboard_character:
    retf

_kb_get_char endp
; -----------------------------------------------------------------------------
; Purpose: Return the combined keyboard or joystick input flags.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _kbinput, _kbscancodes.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_get_kb_or_joy_flags proc far
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
    or      al, 9
check_up_key:
    mov     bl, _kbscancodes+3
    cmp     _kbinput[bx], 0
    jz      short check_up_right_key
    or      al, 1
check_up_right_key:
    mov     bl, _kbscancodes+4
    cmp     _kbinput[bx], 0
    jz      short check_right_key
    or      al, 5
check_right_key:
    mov     bl, _kbscancodes+5
    cmp     _kbinput[bx], 0
    jz      short check_down_right_key
    or      al, 4
check_down_right_key:
    mov     bl, _kbscancodes+6
    cmp     _kbinput[bx], 0
    jz      short check_down_key
    or      al, 6
check_down_key:
    mov     bl, _kbscancodes+7
    cmp     _kbinput[bx], 0
    jz      short check_down_left_key
    or      al, 2
check_down_left_key:
    mov     bl, _kbscancodes+8
    cmp     _kbinput[bx], 0
    jz      short check_left_key
    or      al, 0Ah
check_left_key:
    mov     bl, _kbscancodes+9
    cmp     _kbinput[bx], 0
    jz      short use_joystick_when_no_key
    or      al, 8
use_joystick_when_no_key:
    or      ax, ax
    jnz     short return_input_flags
    call far ptr _get_joy_flags
return_input_flags:
    retf

_get_kb_or_joy_flags endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 305C8. Its internal flow uses check_second_keyboard_scancode, read_joystick_buttons.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _byte_3FE00, _kbinput, _kbscancodes.
; Globals written: none identified.
; /* PLATFORM(input_joy): accesses the joystick game port. */
; -----------------------------------------------------------------------------
_nopsub_305C8 proc far
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
    test    _byte_3FE00, 1
    jz      short return_combined_input_flags
    in      al, dx ; Game I/O port /* PLATFORM(input_joy): accesses the joystick game port. */
    not     al

    and     al, 30h

return_combined_input_flags:
    or      ax, cx
    retf

_nopsub_305C8 endp
; -----------------------------------------------------------------------------
; Purpose: Read and normalize the game-port joystick button and axis state.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _byte_3FE00, _joybutton, _joyflag1, _joyflag2, _joyinput, joystick_x_calibration_ticks, joystick_x_left_threshold, joystick_x_maximum, joystick_x_minimum,
;   joystick_x_right_threshold, joystick_x_sample_maximum, joystick_x_sample_minimum, joystick_y_calibration_ticks, joystick_y_down_threshold, joystick_y_maximum,
;   joystick_y_minimum, joystick_y_sample_maximum, joystick_y_sample_minimum, joystick_y_up_threshold.
; Globals written: _joybutton, _joyflag1, _joyflag2, _joyinput, joystick_x_calibration_ticks, joystick_x_left_threshold, joystick_x_maximum, joystick_x_minimum,
;   joystick_x_right_threshold, joystick_x_sample_maximum, joystick_x_sample_minimum, joystick_x_scale_factor, joystick_y_calibration_ticks, joystick_y_down_threshold,
;   joystick_y_maximum, joystick_y_minimum, joystick_y_sample_maximum, joystick_y_sample_minimum, joystick_y_scale_factor, joystick_y_up_threshold.
; /* PLATFORM(input_joy): accesses the joystick game port. */
; /* PLATFORM(bios): changes the processor interrupt-enable state. */
; -----------------------------------------------------------------------------
_get_joy_flags proc far
    test    _byte_3FE00, 1
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
    db 144
classify_axis_sample:
    test    al, 1
    jnz     short store_x_axis_sample
store_y_axis_sample:
    test    al, 2
    jz      short sample_joystick_axes
    mov     _joyflag2, cx

    and     bl, 1

    jnz     short sample_joystick_axes
    jmp     short calibrate_x_axis
    db 144
store_x_axis_sample:
    mov     _joyflag1, cx

    and     bl, 2

    jnz     short store_y_axis_sample
calibrate_x_axis:
    sti ;/* PLATFORM(bios): changes the processor interrupt-enable state. */
    mov     ax, _joyflag1
    cmp     ax, joystick_x_minimum
    jge     short update_x_axis_maximum
    dec     joystick_x_calibration_ticks
    jle     short set_x_calibration_minimum
    cmp     ax, joystick_x_sample_minimum
    jl      short classify_x_axis_direction
    mov     joystick_x_sample_minimum, ax
    jmp     short classify_x_axis_direction
    ; align 2
    db 144
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
    db 144
update_x_axis_maximum:
    cmp     ax, joystick_x_maximum
    jle     short reset_x_axis_calibration
    dec     joystick_x_calibration_ticks
    jle     short finish_x_axis_calibration
    cmp     ax, joystick_x_sample_maximum
    jge     short classify_x_axis_direction
    mov     joystick_x_sample_maximum, ax
    jmp     short classify_x_axis_direction
    db 144
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
    or      _joyinput, 4
calibrate_y_axis:
    mov     ax, _joyflag2
    cmp     ax, joystick_y_minimum
    jnb     short update_y_axis_maximum
    dec     joystick_y_calibration_ticks
    jle     short set_y_calibration_minimum
    cmp     ax, joystick_y_sample_minimum
    jl      short classify_y_axis_direction
    mov     joystick_y_sample_minimum, ax
    jmp     short classify_y_axis_direction
    db 144
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
    ; align 2
    db 144
set_left_direction_flag:
    or      _joyinput, 8
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
    db 144
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
    or      _joyinput, 2
read_joystick_button_flags:
    in      al, dx ; Game I/O port /* PLATFORM(input_joy): accesses the joystick game port. */
    and     al, _joybutton

    and     al, 30h

    xor     al, 30h
    or      _joyinput, al
    mov     al, _joyinput
    xor     ah, ah
    retf
set_up_direction_flag:
    or      _joyinput, 1
    jmp     short read_joystick_button_flags

_get_joy_flags endp
; -----------------------------------------------------------------------------
; Purpose: Reset the recorded joystick selection state.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _byte_3FE00, joystick_x_maximum, joystick_x_minimum, joystick_y_maximum, joystick_y_minimum.
; -----------------------------------------------------------------------------
_reset_joystick_selection proc far
    mov     _byte_3FE00, 1
    mov     joystick_x_minimum, 50h
    mov     joystick_x_maximum, 0
    mov     joystick_y_minimum, 50h
    mov     joystick_y_maximum, 0
    retf
_reset_joystick_selection endp
; -----------------------------------------------------------------------------
; Purpose: Convert joystick state flags to the selected joystick index.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches BX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: joystick_flags_to_axis_table.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_joystick_flags_to_index proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     bx, [bp+arg_0]

    and     bx, 0Fh

    mov     al, joystick_flags_to_axis_table[bx]
    xor     ah, ah
    pop     bp
    retf
_joystick_flags_to_index endp
; -----------------------------------------------------------------------------
; Purpose: Return a recorded joystick axis value for replay.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _joyflag1, joystick_x_minimum, joystick_x_scale_factor.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_replay_axis_value proc far

    mov     ax, _joyflag1
    sub     ax, joystick_x_minimum
    jge     short clamp_replayed_x_axis
    xor     ax, ax
clamp_replayed_x_axis:
    mul     joystick_x_scale_factor
    mov     al, ah
    mov     ah, dl
    sub     ax, 1Fh
    retf
_replay_axis_value endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 307FA. Its internal flow uses scale_replayed_y_axis.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _joyflag2, joystick_y_minimum, joystick_y_scale_factor.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_307FA proc far
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
    db 0
_nopsub_307FA endp
S012_TEXT ends
end
