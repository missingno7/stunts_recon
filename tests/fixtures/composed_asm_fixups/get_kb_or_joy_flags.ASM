.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn kbscancodes:byte
extrn kbinput:byte
DSEG ends
extrn get_joy_flags:far
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _get_kb_or_joy_flags
_get_kb_or_joy_flags proc far
    xor     ax, ax
    xor     bx, bx
    mov     bl, kbscancodes
    cmp     kbinput[bx], 0
    jz      short loc_30549
    or      al, 10h
loc_30549:
    mov     bl, kbscancodes+1
    cmp     kbinput[bx], 0
    jz      short loc_30556
    or      al, 20h
loc_30556:
    mov     bl, kbscancodes+2
    cmp     kbinput[bx], 0
    jz      short loc_30563
    or      al, 9
loc_30563:
    mov     bl, kbscancodes+3
    cmp     kbinput[bx], 0
    jz      short loc_30570
    or      al, 1
loc_30570:
    mov     bl, kbscancodes+4
    cmp     kbinput[bx], 0
    jz      short loc_3057D
    or      al, 5
loc_3057D:
    mov     bl, kbscancodes+5
    cmp     kbinput[bx], 0
    jz      short loc_3058A
    or      al, 4
loc_3058A:
    mov     bl, kbscancodes+6
    cmp     kbinput[bx], 0
    jz      short loc_30597
    or      al, 6
loc_30597:
    mov     bl, kbscancodes+7
    cmp     kbinput[bx], 0
    jz      short loc_305A4
    or      al, 2
loc_305A4:
    mov     bl, kbscancodes+8
    cmp     kbinput[bx], 0
    jz      short loc_305B1
    or      al, 0Ah
loc_305B1:
    mov     bl, kbscancodes+9
    cmp     kbinput[bx], 0
    jz      short loc_305BE
    or      al, 8
loc_305BE:
    or      ax, ax
    jnz     short locret_305C7
    call far ptr get_joy_flags
locret_305C7:
    retf

_get_kb_or_joy_flags endp
_TEXT ends
end
