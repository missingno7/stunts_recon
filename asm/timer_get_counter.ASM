_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
extrn _timer_callback_counter:dword
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _timer_get_counter
_timer_get_counter proc far
    cli
    mov ax,word ptr ds:[_timer_callback_counter]
    mov dx,word ptr ds:[_timer_callback_counter+2]
    sti
    ret
_timer_get_counter endp
S012_TEXT ends
end
