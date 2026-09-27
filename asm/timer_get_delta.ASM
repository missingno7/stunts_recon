.8086
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
extrn _last_timer_callback_counter:word
extrn _timer_callback_counter:word
extrn _word_2F87A:word
extrn _word_305FC:word
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _timer_get_delta
_timer_get_delta proc far
    mov bx, word ptr ds:[_last_timer_callback_counter]
    mov cx, word ptr ds:[_word_305FC]
    cli
    mov ax, word ptr ds:[_timer_callback_counter]
    mov dx, word ptr ds:[_word_2F87A]
    sti
    mov word ptr ds:[_last_timer_callback_counter], ax
    mov word ptr ds:[_word_305FC], dx
    sub ax, bx
    sbb dx, cx
    retf
_timer_get_delta endp
S012_TEXT ends
end
