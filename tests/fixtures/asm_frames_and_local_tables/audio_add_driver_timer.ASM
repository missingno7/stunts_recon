.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn _timer_reg_callback:far
public _audio_add_driver_timer
_audio_add_driver_timer proc far
    mov bx,6364h
    jmp short check_slot
next_slot:
    mov byte ptr [bx],0
    add bx,4Ch
check_slot:
    cmp bx,6AD0h
    jb short next_slot
    mov word ptr ds:[6AD0h],16h
    mov ax,0403h
    mov dx,seg _TEXT
    push dx
    push ax
    call far ptr _timer_reg_callback
    pop bx
    pop bx
    retf
_audio_add_driver_timer endp
_TEXT ends
end
