.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
DSEG ends
extrn _word_3F886:word
extrn _word_3F87C:word
extrn _word_3F87E:word
extrn _word_3F884:word
extrn _byte_3F881:byte
extrn _word_3F88E:word
extrn _timer_callback_counter:dword
extrn _byte_3F88C:byte
extrn _word_3F88A:word
extrn _word_3F888:word
extrn _timerintr:dword
extrn _word_3F882:word
extrn _byte_3F880:byte
extrn _dword_3F874:dword
S012_TEXT segment byte public 'CODE'
assume cs:S012_TEXT, ds:DSEG
public _timer_intr_callback
public _sub_303BA
_timer_intr_callback proc far
    cli
    push ds
    push es
    push ax
    push bx
    push cx
    push dx
    push si
    push di
    push bp
    mov ax, seg dseg
    mov ds, ax
    sti
    dec word ptr _word_3F886
    jg timer_send_pic_eoi
    inc word ptr _word_3F87C
    jne timer_reload_tick_counter
    inc word ptr _word_3F87E
timer_reload_tick_counter:
    mov ax, word ptr _word_3F884
    mov word ptr _word_3F886, ax
    cmp byte ptr _byte_3F881, 0
    je timer_send_pic_eoi
    call timer_sound_countdown_entry
    jmp short timer_check_callback_enable
    nop
timer_send_pic_eoi:
    mov al, 20h
    out 20h, al
timer_check_callback_enable:
    cmp byte ptr _word_3F88E, 0
    jne timer_clear_reentry_state
    inc word ptr _timer_callback_counter
    jne timer_advance_callback_clock
    inc word ptr _timer_callback_counter+2
timer_advance_callback_clock:
    xor di, di
    cli
    cmp byte ptr _byte_3F88C, 0
    jne timer_count_reentrant_tick
    mov byte ptr _byte_3F88C, 1
    sti
timer_start_callback_scan:
    mov ax, word ptr _timerintr[di+2]
    or ax, ax
    je timer_clear_reentry_state
    call dword ptr _timerintr[di]
    add di, 4
    jmp timer_start_callback_scan
timer_count_reentrant_tick:
    inc word ptr _word_3F88A
    mov ax, word ptr _word_3F88A
    cmp word ptr _word_3F888, ax
    jge timer_restore_irq_frame
    mov word ptr _word_3F888, ax
    jmp short timer_restore_irq_frame
    nop
timer_clear_reentry_state:
    mov byte ptr _byte_3F88C, 0
    mov word ptr _word_3F88A, 0
timer_restore_irq_frame:
    pop bp
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    pop es
    pop ds
    iret
_timer_intr_callback endp
_sub_303BA proc near
timer_sound_countdown_entry:
    cmp byte ptr _byte_3F880, 0
    je timer_chain_previous_timer_irq
    dec word ptr _word_3F882
    jg timer_chain_previous_timer_irq
    mov byte ptr _byte_3F880, 0
    mov byte ptr _byte_3F881, 0
timer_chain_previous_timer_irq:
    pushf
    call dword ptr _dword_3F874
    retn
    db 0 ; single verified trailing alignment/data byte
_sub_303BA endp
S012_TEXT ends
end
