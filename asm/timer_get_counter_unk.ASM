.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn timer_get_counter:far
public _timer_get_counter_unk
_timer_get_counter_unk proc far
    push bp
    mov bp, sp
    sub sp, 4
    call far ptr timer_get_counter
    add ax, word ptr [bp+6]
    adc dx, word ptr [bp+8]
    mov word ptr [bp-4], ax
    mov word ptr [bp-2], dx
wait_for_counter:
    call far ptr timer_get_counter
    cmp dx, word ptr [bp-2]
    jb wait_for_counter
    ja counter_reached
    cmp ax, word ptr [bp-4]
    jb wait_for_counter
counter_reached:
    mov sp, bp
    pop bp
    retf
    db 0
_timer_get_counter_unk endp
_TEXT ends
end
