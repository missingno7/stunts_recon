.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _kb_check
_kb_check proc far

    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jnz     short kb_check_consume_pending_key
    xor     ax, ax
    retf
kb_check_consume_pending_key:
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    jmp     short near ptr _kb_check
_kb_check endp
_TEXT ends
end
