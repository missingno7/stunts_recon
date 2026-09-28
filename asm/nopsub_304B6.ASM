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
    jz      short input_character_no_input_path
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    or      al, al
    jz      short input_character_early_return
    xor     ah, ah
input_character_early_return:
    retf
    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jz      short input_character_no_input_path
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    push    ax
    call far ptr kb_parse_key
    add     sp, 2
    retf
input_character_no_input_path:
    call far ptr get_joy_flags
    mov     bx, ax
    test    ax, 30h
    jz      short character_no_input_path_2
    mov     ax, 0Dh
    jmp     short character_return_if_condition
    db 144
character_no_input_path_2:

    and     bx, 0Fh

    shl     bx, 1
    mov     ax, [bx+4372h]
character_return_if_condition:
    cmp     ax, word_3FB02
    jnz     short character_return_if_nonzero
read_input_character_scan_loop:
    xor     ax, ax
    retf
character_return_if_nonzero:
    cmp     ax, word_3FB04
    jnz     short input_character_when_nonzero
    dec     byte_3FB06
    jg      short read_input_character_scan_loop
    mov     word_3FB02, ax
    retf
input_character_when_nonzero:
    mov     byte_3FB06, 3
    mov     word_3FB04, ax
    jmp     short read_input_character_scan_loop

_nopsub_304B6 endp
_TEXT ends
end
