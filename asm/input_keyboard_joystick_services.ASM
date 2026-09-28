.8086
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
    int     10h
    mov     ah, 0Bh
    mov     bx, 0
    int     10h
    retf
_set_bios_mode3 endp
_kb_parse_key proc far
    push bp
    mov bp, sp
    cli
    cmp byte ptr ds:[key_parser_busy_flag], 0
    je acquire_key_parser_lock
    sti
    pop bp
    retf
acquire_key_parser_lock:
    mov byte ptr ds:[key_parser_busy_flag], 1
    sti
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
_nopsub_304AF proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    xor     ax, ax
    jmp     short store_callback_flags

_nopsub_304AF endp
_nopsub_304B6 proc far
    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jz      short poll_joystick_for_character
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    or      al, al
    jz      short return_empty_key_char
    xor     ah, ah
return_empty_key_char:
    retf
    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jz      short poll_joystick_for_character
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
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
_kb_get_char proc far
    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jnz     short read_buffered_key_char
    xor     ax, ax
    retf
read_buffered_key_char:
    mov     bx, ss
    mov     cx, ds
    cmp     bx, cx
    jnz     short return_keyboard_character
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    push    ax
    call far ptr _kb_parse_key
    add     sp, 2
return_keyboard_character:
    retf

_kb_get_char endp
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
    mov     dx, 201h
    test    _byte_3FE00, 1
    jz      short return_combined_input_flags
    in      al, dx          ; Game I/O port
    not     al

    and     al, 30h

return_combined_input_flags:
    or      ax, cx
    retf

_nopsub_305C8 endp
_get_joy_flags proc far
    test    _byte_3FE00, 1
    jnz     short start_joystick_sampling
    xor     ax, ax
    retf
start_joystick_sampling:
    mov     _joyinput, 0
    mov     dx, 201h
    in      al, dx          ; Game I/O port
    mov     _joybutton, al
    mov     bl, 3
    mov     _joyflag1, 50h ; 'P'
    mov     _joyflag2, 50h ; 'P'
    cli
    out     dx, al          ; Game I/O port
    mov     cx, 14h
joystick_sampling_delay:
    loop    joystick_sampling_delay
    mov     cx, 0
sample_joystick_axes:
    in      al, dx          ; Game I/O port
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
    sti
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
    mov     dx, 201h
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
    mov     dx, 201h
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
    in      al, dx          ; Game I/O port
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
_reset_joystick_selection proc far
    mov     _byte_3FE00, 1
    mov     joystick_x_minimum, 50h
    mov     joystick_x_maximum, 0
    mov     joystick_y_minimum, 50h
    mov     joystick_y_maximum, 0
    retf
_reset_joystick_selection endp
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
