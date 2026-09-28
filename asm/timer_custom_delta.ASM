.8086
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
public _timer_custom_delta
_timer_custom_delta proc far
    push bp
    mov bp, sp
    mov bx, word ptr [bp + 6]
    mov cx, word ptr [bp + 8]
    cli
    mov ax, word ptr ds:[_timer_callback_counter]
    mov dx, word ptr ds:[_timer_callback_counter+2]
    sti
    sub ax, bx
    sbb dx, cx
    pop bp
    retf
_timer_custom_delta endp
S012_TEXT ends
end
