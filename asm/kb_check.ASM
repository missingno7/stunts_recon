.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _kb_check
_kb_check proc far

    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jnz     short loc_30A71
    xor     ax, ax
    retf
loc_30A71:
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    jmp     short near ptr _kb_check
_kb_check endp
_TEXT ends
end
