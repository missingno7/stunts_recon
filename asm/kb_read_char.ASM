.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _kb_read_char
_kb_read_char proc far

    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jnz     short readchar_consume_buffered_key
    xor     ax, ax
    retf
readchar_consume_buffered_key:
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    or      al, al
    jz      short readchar_return_ascii_character
    xor     ah, ah
readchar_return_ascii_character:
    retf
_kb_read_char endp
_TEXT ends
end
