.8086
S012_TEXT segment byte public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:DGROUP
extrn _timer_callback_counter:word
extrn _word_2F87A:word
S012_TEXT segment byte public 'CODE'
public _timer_reset
_timer_reset proc far
    xor     ax, ax
    mov     word ptr _timer_callback_counter, ax
    mov     word ptr _word_2F87A, ax
    retf
_timer_reset endp
S012_TEXT ends
end
