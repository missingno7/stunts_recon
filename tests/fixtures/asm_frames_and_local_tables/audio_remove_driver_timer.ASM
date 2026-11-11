.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn _sub_374DE:far
extrn _timer_remove_callback:far
public _audio_remove_driver_timer
_audio_remove_driver_timer proc far
    push si
    mov si,6364h
    jmp short check_slot
next_slot:
    cmp byte ptr [si],1
    jne short skip_callback
    push word ptr [si+2]
    call far ptr _sub_374DE
    pop bx
skip_callback:
    mov byte ptr [si],0
    add si,4Ch
check_slot:
    cmp si,6AD0h
    jb short next_slot
    mov ax,0403h
    mov dx,seg _TEXT
    push dx
    push ax
    call far ptr _timer_remove_callback
    pop bx
    pop bx
    pop si
    retf
_audio_remove_driver_timer endp
_TEXT ends
end
