.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _kbscancodes:byte
extrn _kbinput:byte
extrn _byte_3FE00:byte
extrn _joyinput:byte
extrn _joybutton:byte
extrn _joyflag1:word
extrn _joyflag2:word
extrn _word_3FB18:word
extrn _word_3FB20:word
extrn _word_3FB1A:word
extrn _word_3FB1C:word
extrn _word_3FB34:word
extrn _word_3FB24:word
extrn _word_3FB22:word
extrn _word_3FB1E:word
extrn _word_3FB26:word
extrn _word_3FB2E:word
extrn _word_3FB28:word
extrn _word_3FB36:word
extrn _word_3FB2A:word
extrn _word_3FB32:word
extrn _word_3FB30:word
extrn _word_3FB2C:word
DSEG ends
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _get_kb_or_joy_flags, _nopsub_305C8, _get_joy_flags
_get_kb_or_joy_flags proc far
    xor     ax, ax
    xor     bx, bx
    mov     bl, _kbscancodes
    cmp     _kbinput[bx], 0
    jz      short kb_scan_0_complete
    or      al, 10h
kb_scan_0_complete:
    mov     bl, _kbscancodes+1
    cmp     _kbinput[bx], 0
    jz      short kb_scan_1_complete
    or      al, 20h
kb_scan_1_complete:
    mov     bl, _kbscancodes+2
    cmp     _kbinput[bx], 0
    jz      short kb_scan_2_complete
    or      al, 9
kb_scan_2_complete:
    mov     bl, _kbscancodes+3
    cmp     _kbinput[bx], 0
    jz      short kb_scan_3_complete
    or      al, 1
kb_scan_3_complete:
    mov     bl, _kbscancodes+4
    cmp     _kbinput[bx], 0
    jz      short kb_scan_4_complete
    or      al, 5
kb_scan_4_complete:
    mov     bl, _kbscancodes+5
    cmp     _kbinput[bx], 0
    jz      short kb_scan_5_complete
    or      al, 4
kb_scan_5_complete:
    mov     bl, _kbscancodes+6
    cmp     _kbinput[bx], 0
    jz      short kb_scan_6_complete
    or      al, 6
kb_scan_6_complete:
    mov     bl, _kbscancodes+7
    cmp     _kbinput[bx], 0
    jz      short kb_scan_7_complete
    or      al, 2
kb_scan_7_complete:
    mov     bl, _kbscancodes+8
    cmp     _kbinput[bx], 0
    jz      short kb_scan_8_complete
    or      al, 0Ah
kb_scan_8_complete:
    mov     bl, _kbscancodes+9
    cmp     _kbinput[bx], 0
    jz      short kb_scan_9_complete
    or      al, 8
kb_scan_9_complete:
    or      ax, ax
    jnz     short kb_fallback_to_joystick
    call far ptr _get_joy_flags
kb_fallback_to_joystick:
    retf

_get_kb_or_joy_flags endp
_nopsub_305C8 proc far
    xor     cx, cx
    xor     bx, bx
    mov     bl, _kbscancodes
    cmp     byte ptr [bx+446Ah], 0
    jz      short joy_keys_scan_0_complete
    or      cl, 10h
joy_keys_scan_0_complete:
    mov     bl, _kbscancodes+1
    cmp     byte ptr [bx+446Ah], 0
    jz      short joy_keys_scan_1_complete
    or      cl, 20h
joy_keys_scan_1_complete:
    xor     ax, ax
    mov     dx, 201h
    test    _byte_3FE00, 1
    jz      short joy_port_read_skip_disabled
    in      al, dx          ; Game I/O port
    not     al

    and     al, 30h

joy_port_read_skip_disabled:
    or      ax, cx
    retf

_nopsub_305C8 endp
_get_joy_flags proc far
    test    _byte_3FE00, 1
    jnz     short joy_return_when_disabled
    xor     ax, ax
    retf
joy_return_when_disabled:
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
joy_axis_settle_delay_loop:
    loop    joy_axis_settle_delay_loop
    mov     cx, 0
joy_axis_sample_timeout_loop:
    in      al, dx          ; Game I/O port
    and     al, bl
    xor     al, bl
    jnz     short joy_sample_first_axis
    inc     cx
    cmp     cx, 0FA0h
    jl      short joy_axis_sample_timeout_loop
    jmp     short joy_process_axis_samples
    db 144
joy_sample_first_axis:
    test    al, 1
    jnz     short joy_capture_first_axis
joy_sample_second_axis:
    test    al, 2
    jz      short joy_axis_sample_timeout_loop
    mov     _joyflag2, cx

    and     bl, 1

    jnz     short joy_axis_sample_timeout_loop
    jmp     short joy_process_axis_samples
    db 144
joy_capture_first_axis:
    mov     _joyflag1, cx

    and     bl, 2

    jnz     short joy_sample_second_axis
joy_process_axis_samples:
    sti
    mov     ax, _joyflag1
    cmp     ax, _word_3FB18
    jge     short joy_process_second_axis
    dec     _word_3FB20
    jle     short joy_reset_first_axis_range
    cmp     ax, _word_3FB1A
    jl      short joy_classify_first_axis
    mov     _word_3FB1A, ax
    jmp     short joy_classify_first_axis
    ; align 2
    db 144
joy_reset_first_axis_range:
    mov     bx, _word_3FB1A
    mov     _word_3FB18, bx
    mov     bx, _word_3FB1C
joy_calculate_first_axis_scale:
    sub     bx, _word_3FB18
    jle     short joy_store_first_axis_thresholds
    xor     dx, dx
    mov     ax, 4000h
    div     bx
    mov     _word_3FB34, ax
joy_store_first_axis_thresholds:
    mov     ax, bx
    mov     dx, 201h
    shr     ax, 1
    mov     bx, ax
    shr     bx, 1
    add     ax, _word_3FB18
    add     ax, bx
    mov     _word_3FB24, ax
    sub     ax, bx
    sub     ax, bx
    mov     _word_3FB22, ax
    mov     ax, _joyflag1
    jmp     short joy_reset_axis2_thresholds
    db 144
joy_process_second_axis:
    cmp     ax, _word_3FB1C
    jle     short joy_reset_axis2_thresholds
    dec     _word_3FB20
    jle     short joy_reset_second_axis_range
    cmp     ax, _word_3FB1E
    jge     short joy_classify_first_axis
    mov     _word_3FB1E, ax
    jmp     short joy_classify_first_axis
    db 144
joy_reset_second_axis_range:
    mov     bx, _word_3FB1E
    mov     _word_3FB1C, bx
    jmp     short joy_calculate_first_axis_scale
joy_reset_axis2_thresholds:
    mov     _word_3FB20, 14h
    mov     _word_3FB1E, 4E20h
    mov     _word_3FB1A, 0
joy_classify_first_axis:
    cmp     ax, _word_3FB22
    jl      short joy_compare_second_axis_bands
    cmp     ax, _word_3FB24
    jl      short joy_classify_first_axis_high
    or      _joyinput, 4
joy_classify_first_axis_high:
    mov     ax, _joyflag2
    cmp     ax, _word_3FB26
    jnb     short joy_second_axis_low_limit
    dec     _word_3FB2E
    jle     short joy_reset_first_axis_low
    cmp     ax, _word_3FB28
    jl      short joy_classify_second_axis
    mov     _word_3FB28, ax
    jmp     short joy_classify_second_axis
    db 144
joy_reset_first_axis_low:
    mov     bx, _word_3FB28
    mov     _word_3FB26, bx
    mov     bx, _word_3FB2A
joy_calculate_second_axis_scale:
    sub     bx, _word_3FB26
    jle     short joy_store_axis2_thresholds
    xor     dx, dx
    mov     ax, 4000h
    div     bx
    mov     _word_3FB36, ax
joy_store_axis2_thresholds:
    mov     ax, bx
    mov     dx, 201h
    shr     ax, 1
    mov     bx, ax
    shr     bx, 1
    add     ax, _word_3FB26
    add     ax, bx
    mov     _word_3FB32, ax
    sub     ax, bx
    sub     ax, bx
    mov     _word_3FB30, ax
    mov     ax, _joyflag2
    jmp     short joy_reset_axis2_calibration
    ; align 2
    db 144
joy_compare_second_axis_bands:
    or      _joyinput, 8
    jmp     short joy_classify_first_axis_high
joy_second_axis_low_limit:
    cmp     ax, _word_3FB2A
    jle     short joy_reset_axis2_calibration
    dec     _word_3FB2E
    jz      short joy_reset_second_axis_low
joy_update_second_axis_low:
    cmp     ax, _word_3FB2C
    jge     short joy_classify_second_axis
    mov     _word_3FB2C, ax
    jmp     short joy_classify_second_axis
    db 144
joy_reset_second_axis_low:
    mov     bx, _word_3FB2C
    mov     _word_3FB2A, bx
    jmp     short joy_calculate_second_axis_scale
joy_reset_axis2_calibration:
    mov     _word_3FB2E, 14h
    mov     _word_3FB2C, 4E20h
    mov     _word_3FB28, 0
joy_classify_second_axis:
    cmp     ax, _word_3FB30
    jb      short joy_first_axis_low_limit
    cmp     ax, _word_3FB32
    jb      short joy_read_buttons
    or      _joyinput, 2
joy_read_buttons:
    in      al, dx          ; Game I/O port
    and     al, _joybutton

    and     al, 30h

    xor     al, 30h
    or      _joyinput, al
    mov     al, _joyinput
    xor     ah, ah
    retf
joy_first_axis_low_limit:
    or      _joyinput, 1
    jmp     short joy_read_buttons

_get_joy_flags endp
_TEXT ends
end
