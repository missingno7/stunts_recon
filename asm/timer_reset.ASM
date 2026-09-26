.8086
_TEXT segment word public 'CODE'
_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:_TEXT, ds:DGROUP
extrn _timer_callback_counter:word
extrn _word_2F87A:word
_TEXT segment word public 'CODE'
public _timer_reset
_timer_reset proc far
    xor     ax, ax
    mov     word ptr _timer_callback_counter, ax
    mov     word ptr _word_2F87A, ax
    retf
_timer_reset endp
_TEXT ends
end
