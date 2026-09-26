.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn word_3FB02:word
extrn word_3FB04:word
extrn byte_3FB06:byte
DSEG ends
extrn kb_parse_key:far
extrn get_joy_flags:far
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _nopsub_304B6
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
    call far ptr kb_parse_key
    add     sp, 2
    retf
loc_304DB:
    call far ptr get_joy_flags
    mov     bx, ax
    test    ax, 30h
    jz      short loc_304ED
    mov     ax, 0Dh
    jmp     short loc_304F6
    db 144
loc_304ED:

    and     bx, 0Fh

    shl     bx, 1
    mov     ax, [bx+4372h]
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
_TEXT ends
end
