.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _kb_read_char
_kb_read_char proc far

    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jnz     short loc_30A2A
    xor     ax, ax
    retf
loc_30A2A:
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    or      al, al
    jz      short locret_30A34
    xor     ah, ah
locret_30A34:
    retf
_kb_read_char endp
_TEXT ends
end
