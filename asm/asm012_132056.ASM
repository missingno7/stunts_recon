.8086
DGROUP group _DATA
assume ds:DGROUP
_DATA segment word public 'DATA'
_callbackflags db 128 dup (0)
_callbackflags2 db 134 dup (0)
_byte_2F9E0 db 2 dup (0)
_callbacks db 256 dup (0)
word_2FAE2 dw 0, 04800h, 05000h, 0, 04D00h, 04900h, 05100h, 0
    dw 04B00h, 04700h, 04F00h, 0, 0, 0, 0, 0
word_3FB02 dw 0
word_3FB04 dw 0
byte_3FB06 db 2 dup (0)
_kbscancodes db 039h, 01Ch, 047h, 048h, 049h, 04Dh, 051h, 050h, 04Fh, 04Bh
_joyflag1 dw 0
_joyflag2 dw 0
_joybutton db 0
_joyinput db 0
_word_3FB18 dw 00050h
_word_3FB1A dw 0
_word_3FB1C dw 0
_word_3FB1E dw 0
_word_3FB20 dw 00014h
_word_3FB22 dw 0
_word_3FB24 dw 0
_word_3FB26 dw 00050h
_word_3FB28 dw 0
_word_3FB2A dw 0
_word_3FB2C dw 0
_word_3FB2E dw 00014h
_word_3FB30 dw 0
_word_3FB32 dw 0
_word_3FB34 dw 0
_word_3FB36 dw 0
_byte_3FB38 db 0, 1, 5, 0, 3, 2, 4, 3, 7, 8, 6, 7, 0, 1, 5, 0
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
    cmp byte ptr ds:[_byte_2F9E0], 0
    je loc_20412
    sti
    pop bp
    retf
loc_20412:
    mov byte ptr ds:[_byte_2F9E0], 1
    sti
    mov bx, word ptr [bp + 6]
    or bl, bl
    je loc_20441
    and bx, 7Fh
    mov word ptr [bp + 6], bx
    mov bl, byte ptr _callbackflags[bx]
loc_20429:
    dec bx
    jl loc_20454
    shl bx, 1
    push di
    push si
    shl bx, 1
    call dword ptr _callbacks[bx]
    pop si
    pop di
    xor ax, ax
    mov byte ptr ds:[_byte_2F9E0], 0
    pop bp
    retf
loc_20441:
    mov bl, bh
    xor bh, bh
    cmp bx, 84h
    jl loc_2044E
    mov bx, 84h
loc_2044E:
    mov bl, byte ptr _callbackflags2[bx]
    jmp loc_20429
loc_20454:
    mov ax, word ptr [bp + 6]
    mov byte ptr ds:[_byte_2F9E0], 0
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
loc_3046D:
    cmp     [bx], ax
    jnz     short loc_30476
    cmp     [bx+2], dx
    jz      short loc_30488
loc_30476:
    cmp     word ptr [bx+2], 0
    jz      short loc_30483
    add     bx, 4
    loop    loc_3046D
    pop     bp
    retf
loc_30483:
    mov     [bx], ax
    mov     [bx+2], dx
loc_30488:
    mov     ax, 41h ; 'A'
    sub     ax, cx
loc_3048D:
    mov     bx, [bp+arg_0]
    or      bl, bl
    jz      short loc_3049F
    cmp     bx, 7Fh ; ''
    jg      short loc_3049D
    mov     _callbackflags[bx], al
loc_3049D:
    pop     bp
    retf
loc_3049F:
    mov     bl, bh
    xor     bh, bh
    cmp     bx, 84h ; ''
    jg      short loc_3049D
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
    jmp     short loc_3048D

_nopsub_304AF endp
_nopsub_304B6 proc far
    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jz      short loc_304DB
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    or      al, al
    jz      short locret_304C6
    xor     ah, ah
locret_304C6:
    retf
    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jz      short loc_304DB
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    push    ax
    call far ptr _kb_parse_key
    add     sp, 2
    retf
loc_304DB:
    call far ptr _get_joy_flags
    mov     bx, ax
    test    ax, 30h
    jz      short loc_304ED
    mov     ax, 0Dh
    jmp     short loc_304F6
    db 144
loc_304ED:

    and     bx, 0Fh

    shl     bx, 1
    mov     ax, word_2FAE2[bx]
loc_304F6:
    cmp     ax, word_3FB02
    jnz     short loc_304FF
loc_304FC:
    xor     ax, ax
    retf
loc_304FF:
    cmp     ax, word_3FB04
    jnz     short loc_3050F
    dec     byte_3FB06
    jg      short loc_304FC
    mov     word_3FB02, ax
    retf
loc_3050F:
    mov     byte_3FB06, 3
    mov     word_3FB04, ax
    jmp     short loc_304FC

_nopsub_304B6 endp
_kb_get_char proc far
    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jnz     short loc_30522
    xor     ax, ax
    retf
loc_30522:
    mov     bx, ss
    mov     cx, ds
    cmp     bx, cx
    jnz     short locret_30537
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    push    ax
    call far ptr _kb_parse_key
    add     sp, 2
locret_30537:
    retf

_kb_get_char endp
_get_kb_or_joy_flags proc far
    xor     ax, ax
    xor     bx, bx
    mov     bl, _kbscancodes
    cmp     _kbinput[bx], 0
    jz      short loc_30549
    or      al, 10h
loc_30549:
    mov     bl, _kbscancodes+1
    cmp     _kbinput[bx], 0
    jz      short loc_30556
    or      al, 20h
loc_30556:
    mov     bl, _kbscancodes+2
    cmp     _kbinput[bx], 0
    jz      short loc_30563
    or      al, 9
loc_30563:
    mov     bl, _kbscancodes+3
    cmp     _kbinput[bx], 0
    jz      short loc_30570
    or      al, 1
loc_30570:
    mov     bl, _kbscancodes+4
    cmp     _kbinput[bx], 0
    jz      short loc_3057D
    or      al, 5
loc_3057D:
    mov     bl, _kbscancodes+5
    cmp     _kbinput[bx], 0
    jz      short loc_3058A
    or      al, 4
loc_3058A:
    mov     bl, _kbscancodes+6
    cmp     _kbinput[bx], 0
    jz      short loc_30597
    or      al, 6
loc_30597:
    mov     bl, _kbscancodes+7
    cmp     _kbinput[bx], 0
    jz      short loc_305A4
    or      al, 2
loc_305A4:
    mov     bl, _kbscancodes+8
    cmp     _kbinput[bx], 0
    jz      short loc_305B1
    or      al, 0Ah
loc_305B1:
    mov     bl, _kbscancodes+9
    cmp     _kbinput[bx], 0
    jz      short loc_305BE
    or      al, 8
loc_305BE:
    or      ax, ax
    jnz     short locret_305C7
    call far ptr _get_joy_flags
locret_305C7:
    retf

_get_kb_or_joy_flags endp
_nopsub_305C8 proc far
    xor     cx, cx
    xor     bx, bx
    mov     bl, _kbscancodes
    cmp     byte ptr _kbinput[bx], 0
    jz      short loc_305DA
    or      cl, 10h
loc_305DA:
    mov     bl, _kbscancodes+1
    cmp     byte ptr _kbinput[bx], 0
    jz      short loc_305E8
    or      cl, 20h
loc_305E8:
    xor     ax, ax
    mov     dx, 201h
    test    _byte_3FE00, 1
    jz      short loc_305F9
    in      al, dx          ; Game I/O port
    not     al

    and     al, 30h

loc_305F9:
    or      ax, cx
    retf

_nopsub_305C8 endp
_get_joy_flags proc far
    test    _byte_3FE00, 1
    jnz     short loc_30606
    xor     ax, ax
    retf
loc_30606:
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
loc_30625:
    loop    loc_30625
    mov     cx, 0
loc_3062A:
    in      al, dx          ; Game I/O port
    and     al, bl
    xor     al, bl
    jnz     short loc_3063B
    inc     cx
    cmp     cx, 0FA0h
    jl      short loc_3062A
    jmp     short loc_30658
    db 144
loc_3063B:
    test    al, 1
    jnz     short loc_3064F
loc_3063F:
    test    al, 2
    jz      short loc_3062A
    mov     _joyflag2, cx

    and     bl, 1

    jnz     short loc_3062A
    jmp     short loc_30658
    db 144
loc_3064F:
    mov     _joyflag1, cx

    and     bl, 2

    jnz     short loc_3063F
loc_30658:
    sti
    mov     ax, _joyflag1
    cmp     ax, _word_3FB18
    jge     short loc_306B1
    dec     _word_3FB20
    jle     short loc_30674
    cmp     ax, _word_3FB1A
    jl      short loc_306E5
    mov     _word_3FB1A, ax
    jmp     short loc_306E5
    ; align 2
    db 144
loc_30674:
    mov     bx, _word_3FB1A
    mov     _word_3FB18, bx
    mov     bx, _word_3FB1C
loc_30680:
    sub     bx, _word_3FB18
    jle     short loc_30690
    xor     dx, dx
    mov     ax, 4000h
    div     bx
    mov     _word_3FB34, ax
loc_30690:
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
    jmp     short loc_306D3
    db 144
loc_306B1:
    cmp     ax, _word_3FB1C
    jle     short loc_306D3
    dec     _word_3FB20
    jle     short loc_306C9
    cmp     ax, _word_3FB1E
    jge     short loc_306E5
    mov     _word_3FB1E, ax
    jmp     short loc_306E5
    db 144
loc_306C9:
    mov     bx, _word_3FB1E
    mov     _word_3FB1C, bx
    jmp     short loc_30680
loc_306D3:
    mov     _word_3FB20, 14h
    mov     _word_3FB1E, 4E20h
    mov     _word_3FB1A, 0
loc_306E5:
    cmp     ax, _word_3FB22
    jl      short loc_3074E
    cmp     ax, _word_3FB24
    jl      short loc_306F6
    or      _joyinput, 4
loc_306F6:
    mov     ax, _joyflag2
    cmp     ax, _word_3FB26
    jnb     short loc_30755
    dec     _word_3FB2E
    jle     short loc_30711
    cmp     ax, _word_3FB28
    jl      short loc_30789
    mov     _word_3FB28, ax
    jmp     short loc_30789
    db 144
loc_30711:
    mov     bx, _word_3FB28
    mov     _word_3FB26, bx
    mov     bx, _word_3FB2A
loc_3071D:
    sub     bx, _word_3FB26
    jle     short loc_3072D
    xor     dx, dx
    mov     ax, 4000h
    div     bx
    mov     _word_3FB36, ax
loc_3072D:
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
    jmp     short loc_30777
    ; align 2
    db 144
loc_3074E:
    or      _joyinput, 8
    jmp     short loc_306F6
loc_30755:
    cmp     ax, _word_3FB2A
    jle     short loc_30777
    dec     _word_3FB2E
    jz      short loc_3076D
loc_30761:
    cmp     ax, _word_3FB2C
    jge     short loc_30789
    mov     _word_3FB2C, ax
    jmp     short loc_30789
    db 144
loc_3076D:
    mov     bx, _word_3FB2C
    mov     _word_3FB2A, bx
    jmp     short loc_3071D
loc_30777:
    mov     _word_3FB2E, 14h
    mov     _word_3FB2C, 4E20h
    mov     _word_3FB28, 0
loc_30789:
    cmp     ax, _word_3FB30
    jb      short loc_307AD
    cmp     ax, _word_3FB32
    jb      short loc_3079A
    or      _joyinput, 2
loc_3079A:
    in      al, dx          ; Game I/O port
    and     al, _joybutton

    and     al, 30h

    xor     al, 30h
    or      _joyinput, al
    mov     al, _joyinput
    xor     ah, ah
    retf
loc_307AD:
    or      _joyinput, 1
    jmp     short loc_3079A

_get_joy_flags endp
_reset_joystick_selection proc far
    mov     _byte_3FE00, 1
    mov     _word_3FB18, 50h
    mov     _word_3FB1C, 0
    mov     _word_3FB26, 50h
    mov     _word_3FB2A, 0
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

    mov     al, _byte_3FB38[bx]
    xor     ah, ah
    pop     bp
    retf
_joystick_flags_to_index endp
_replay_axis_value proc far

    mov     ax, _joyflag1
    sub     ax, _word_3FB18
    jge     short loc_307EE
    xor     ax, ax
loc_307EE:
    mul     _word_3FB34
    mov     al, ah
    mov     ah, dl
    sub     ax, 1Fh
    retf
_replay_axis_value endp
_nopsub_307FA proc far
    mov     ax, _joyflag2
    sub     ax, _word_3FB26
    jge     short loc_30805
    xor     ax, ax
loc_30805:
    mul     _word_3FB36
    mov     al, ah
    mov     ah, dl
    sub     ax, 1Fh
    retf
    db 0
_nopsub_307FA endp
S012_TEXT ends
end
