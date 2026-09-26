.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn kb_parse_key:far
public _kb_get_char
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
    call far ptr kb_parse_key
    add     sp, 2
locret_30537:
    retf

_kb_get_char endp
_TEXT ends
end
