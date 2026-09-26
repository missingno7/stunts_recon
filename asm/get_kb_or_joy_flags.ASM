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
    cmp     byte ptr [bx+446Ah], 0
    jz      short loc_305DA
    or      cl, 10h
loc_305DA:
    mov     bl, _kbscancodes+1
    cmp     byte ptr [bx+446Ah], 0
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
_TEXT ends
end
