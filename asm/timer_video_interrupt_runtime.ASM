.8086
name asm012_131254
DGROUP group DSEG, _DATA
DSEG segment byte public 'STUNTSD'
DSEG ends
assume ds:DGROUP
_DATA segment word public 'DATA'
public _byte_3F868, _previous_timer_irq_vector, _timer_callback_counter
public _timer_elapsed_ticks_low
public _timer_elapsed_ticks_high
public _timer_sound_countdown_active
public _timer_sound_countdown_enabled
public _timer_sound_countdown_ticks
public _timer_interrupt_reload_value
public _bios_tick_countdown
public _timer_callback_reentry_peak
public _timer_callback_reentry_depth
public _timer_callback_busy, _word_3F88E, _timerintr, _aNoRoomLeftOnTimerInterru
_byte_3F868 db 97, 80, 82, 15, 25, 6, 25, 25, 2, 13, 11, 12
_previous_timer_irq_vector dd 0
_timer_callback_counter dd 0
_timer_elapsed_ticks_low dw 0
_timer_elapsed_ticks_high dw 0
_timer_sound_countdown_active db 0
_timer_sound_countdown_enabled db 0
_timer_sound_countdown_ticks dw 0
_timer_interrupt_reload_value dw 0
_bios_tick_countdown dw 0
_timer_callback_reentry_peak dw 0
_timer_callback_reentry_depth dw 0
_timer_callback_busy db 0
db 3
_word_3F88E dw 0
_timerintr dd 0
db 24 dup (0)
_aNoRoomLeftOnTimerInterru db "NO ROOM LEFT ON TIMER INTERRUPT ROUTINE LIST", 13, 0
_DATA ends
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _polradius2d
public _video_set_mode7
public _nopsub_30180
public _timer_setup_interrupt
public _audio_stop_unknown
public _timer_reg_callback
public _timer_remove_callback
public _compare_ds_ss
public _timer_intr_callback
public _sub_303BA
extrn _cosfast:far
extrn _polang:far
extrn _sinfast:far
extrn _video_mode4_active:byte
extrn _set_bios_mode3:far
extrn _add_exit_handler:far
extrn _fatal_error:far

; polarRadius2D: _polradius2d
_polradius2d proc far
    arg_z = word ptr 6
    arg_y = word ptr 8
    push bp
    mov bp, sp
    push word ptr [bp+arg_y]
    push word ptr [bp+arg_z]
    call far ptr _polang
    add sp, 4
    or ax, ax
    jge short radius_reflect_angle_into_range
    neg ax
radius_reflect_angle_into_range:
    cmp ax, 100h
    jl short radius_choose_cosine_branch
    sub ax, 200h
    neg ax
radius_choose_cosine_branch:
    cmp ax, 80h
    jg short radius_choose_sine_branch
    push ax
    call far ptr _cosfast
    add sp, 2
    mov bx, ax
    mov dx, word ptr [bp+arg_y]
    or dx, dx
    jge short radius_scale_absolute_z
    neg dx
radius_scale_absolute_z:
    xor ax, ax
    sar dx, 1
    rcr ax, 1
    sar dx, 1
    rcr ax, 1
    div bx
    pop bp
    retf
radius_choose_sine_branch:
    push ax
    call far ptr _sinfast
    add sp, 2
    mov bx, ax
    mov dx, word ptr [bp+arg_z]
    or dx, dx
    jge short radius_scale_absolute_x
    neg dx
radius_scale_absolute_x:
    xor ax, ax
    sar dx, 1
    rcr ax, 1
    sar dx, 1
    rcr ax, 1
    div bx
    pop bp
    retf
_polradius2d endp

; video_set_mode7: _video_set_mode7
_video_set_mode7 proc far
    push di
    cmp byte ptr ds:[_video_mode4_active], 0
    jne mode7_program_crtc
    call far ptr _set_bios_mode3
    pop di
    retf
mode7_program_crtc:
    mov ax, 40h
    mov es, ax
    mov ax, word ptr es:[10h]
    and ax, 0FFCFh
    or ax, 30h
    mov word ptr es:[10h], ax
    mov al, 3
    mov dx, 3BFh
    out dx, al
    mov al, 20h
    mov dx, 3B8h
    out dx, al
    mov cx, 0Ch
    mov dx, 3B4h
    xor bx, bx
mode7_write_crtc_register_loop:
    mov al, bl
    out dx, al
    inc dx
    mov al, byte ptr [bx + 40F8h]
    out dx, al
    dec dx
    inc bx
    loop mode7_write_crtc_register_loop
    cld
    mov ax, 0B800h
    mov es, ax
    xor di, di
    mov cx, 4000h
    xor ax, ax
    rep stosw
    mov al, 28h
    mov dx, 3B8h
    out dx, al
    mov ah, 0
    mov al, 7
    int 10h
    pop di
    retf
_video_set_mode7 endp

; nopsub_30180: _nopsub_30180
_nopsub_30180 proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    mov     dx, 2E9Ch
    mov     _timer_interrupt_reload_value, 5
    mov     _bios_tick_countdown, 5
    mov     _timer_sound_countdown_ticks, 64h ; 'd'
    mov     _timer_sound_countdown_active, 1
    jmp     short shared_setup
    ; align 2
    db 144

_nopsub_30180 endp

; nopsub_30180: _timer_setup_interrupt
_timer_setup_interrupt proc far
 push bp
 mov dx,2E9Ch
 mov word ptr ds:[_timer_interrupt_reload_value],5
 mov word ptr ds:[_bios_tick_countdown],5
 mov byte ptr ds:[_timer_sound_countdown_active],0
 mov byte ptr ds:[_timer_sound_countdown_enabled],1
 jmp shared_setup
 push bp
 mov bp,sp
 mov dx,1
 xor ax,ax
 idiv word ptr [bp+6]
 mov word ptr ds:[_timer_interrupt_reload_value],ax
 mov word ptr ds:[_bios_tick_countdown],ax
 mov dx,word ptr [bp+6]
 mov byte ptr ds:[_timer_sound_countdown_active],0
 mov byte ptr ds:[_timer_sound_countdown_enabled],1
 jmp shared_setup
 push bp
 mov bp,sp
 mov dx,1
 xor ax,ax
 idiv word ptr [bp+6]
 mov word ptr ds:[_timer_interrupt_reload_value],ax
 mov word ptr ds:[_bios_tick_countdown],ax
 mov dx,word ptr [bp+6]
 mov word ptr ds:[_timer_sound_countdown_ticks],64h
 mov byte ptr ds:[_timer_sound_countdown_active],1
shared_setup:
 cli
 mov byte ptr ds:[_timer_callback_busy],0
 mov word ptr ds:[_timerintr],0
 mov word ptr ds:[_timerintr+2],0
 sti
 in al,61h
 and al,0FCh
 out 61h,al
 mov al,0B6h
 out 43h,al
 in al,21h
 or al,3
 out 21h,al
 xor ax,ax
 mov es,ax
 cli
 mov ax,word ptr es:[20h]
 cmp ax,1909h
 je setup_after_old_off
 mov word ptr ds:[_previous_timer_irq_vector],ax
setup_after_old_off:
 mov ax,word ptr es:[22h]
 mov bx,cs
 cmp ax,bx
 je setup_after_old_seg
 mov word ptr ds:[_previous_timer_irq_vector+2],ax
 mov word ptr es:[20h],1909h
 mov word ptr es:[22h],cs
setup_after_old_seg:
 in al,21h
 and al,0FCh
 out 21h,al
 sti
 mov al,dl
 out 40h,al
 mov al,dh
 out 40h,al
 push cs
 mov ax,1848h
 push ax
 call far ptr _add_exit_handler
 add sp,4
 pop bp
 ret

_timer_setup_interrupt endp

; audio_stop_unk: _audio_stop_unknown
_audio_stop_unknown proc far

    xor     ax, ax
    mov     es, ax
    mov     ax, es:22h
    mov     dx, cs
    cmp     ax, dx
    jz      short timer_restore_owner_check
    retf
timer_restore_owner_check:
    mov     ax, es:20h
    cmp     ax, 1909h
    jz      short timer_restore_prev_irq_vector
    retf
timer_restore_prev_irq_vector:
    in      al, 21h         ; Interrupt controller, 8259A.
    or      al, 3
    out     21h, al         ; Interrupt controller, 8259A.
    cli
    mov     ax, word ptr _previous_timer_irq_vector
    mov     es:20h, ax
    mov     ax, word ptr _previous_timer_irq_vector+2
    mov     es:22h, ax
    in      al, 21h         ; Interrupt controller, 8259A.

    and     al, 0FCh

    out     21h, al         ; Interrupt controller, 8259A.
    sti
    mov     al, 0
    out     40h, al         ; Timer 8253-5 (AT: 8254.2).
    out     40h, al         ; Timer 8253-5 (AT: 8254.2).
    in      al, 61h         ; PC/XT PPI port B bits:

    and     al, 0FCh

    out     61h, al         ; PC/XT PPI port B bits:
    retf
_audio_stop_unknown endp

; timer_reg_callback: _timer_reg_callback
_timer_reg_callback proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     cx, 5
    mov     bx, offset DGROUP:_timerintr
timer_callback_find_free_slot:
    cmp     word ptr [bx+2], 0
    jz      short timer_callback_slot_available
    add     bx, 4
    loop    timer_callback_find_free_slot
    mov     ax, offset DGROUP:_aNoRoomLeftOnTimerInterru; "NO ROOM LEFT ON TIMER INTERRUPT ROUTINE"...
    push    ax
    call    far ptr _fatal_error
timer_callback_slot_available:
    mov     ax, [bp+arg_0]
    mov     [bx], ax
    mov     word ptr [bx+2], 0
    mov     ax, [bp+arg_2]
    mov     [bx+2], ax
    mov     word ptr [bx+6], 0
    pop     bp
    retf
_timer_reg_callback endp

; timer_remove_callback: _timer_remove_callback
_timer_remove_callback proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     cx, 5
    mov     bx, offset DGROUP:_timerintr
    mov     ax, [bp+arg_0]
    mov     dx, [bp+arg_2]
timer_remove_search_slot_loop:
    cmp     [bx], ax
    jnz     short timer_remove_advance_slot
    cmp     [bx+2], dx
    jz      short timer_remove_disable_interrupts
timer_remove_advance_slot:
    add     bx, 4
    loop    timer_remove_search_slot_loop
    pop     bp
    retf
timer_remove_disable_interrupts:
    cli
    dec     cx
    jz      short timer_remove_clear_tail_slot
timer_remove_shift_slots:
    mov     ax, [bx+4]
    mov     [bx], ax
    mov     ax, [bx+6]
    mov     [bx+2], ax
    add     bx, 4
    loop    timer_remove_shift_slots
timer_remove_clear_tail_slot:
    mov     word ptr [bx], 0
    mov     word ptr [bx+2], 0
    sti
    pop     bp
    retf
_timer_remove_callback endp

; compare_ds_ss: _compare_ds_ss
_compare_ds_ss proc far

    xor     ax, ax
    mov     bx, ss
    mov     dx, ds
    cmp     bx, dx
    jnz     short compare_equal_segment_return
    inc     ax
compare_equal_segment_return:
    retf
_compare_ds_ss endp

; timer_intr_group: _timer_intr_callback
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
    dec word ptr _bios_tick_countdown
    jg timer_send_pic_eoi
    inc word ptr _timer_elapsed_ticks_low
    jne timer_reload_tick_counter
    inc word ptr _timer_elapsed_ticks_high
timer_reload_tick_counter:
    mov ax, word ptr _timer_interrupt_reload_value
    mov word ptr _bios_tick_countdown, ax
    cmp byte ptr _timer_sound_countdown_enabled, 0
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
    cmp byte ptr _timer_callback_busy, 0
    jne timer_count_reentrant_tick
    mov byte ptr _timer_callback_busy, 1
    sti
timer_start_callback_scan:
    mov ax, word ptr _timerintr[di+2]
    or ax, ax
    je timer_clear_reentry_state
    call dword ptr _timerintr[di]
    add di, 4
    jmp timer_start_callback_scan
timer_count_reentrant_tick:
    inc word ptr _timer_callback_reentry_depth
    mov ax, word ptr _timer_callback_reentry_depth
    cmp word ptr _timer_callback_reentry_peak, ax
    jge timer_restore_irq_frame
    mov word ptr _timer_callback_reentry_peak, ax
    jmp short timer_restore_irq_frame
    nop
timer_clear_reentry_state:
    mov byte ptr _timer_callback_busy, 0
    mov word ptr _timer_callback_reentry_depth, 0
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

; timer_intr_group: _sub_303BA
_sub_303BA proc near
timer_sound_countdown_entry:
    cmp byte ptr _timer_sound_countdown_active, 0
    je timer_chain_previous_timer_irq
    dec word ptr _timer_sound_countdown_ticks
    jg timer_chain_previous_timer_irq
    mov byte ptr _timer_sound_countdown_active, 0
    mov byte ptr _timer_sound_countdown_enabled, 0
timer_chain_previous_timer_irq:
    pushf
    call dword ptr _previous_timer_irq_vector
    retn
    db 0 ; single verified trailing alignment/data byte
_sub_303BA endp
S012_TEXT ends
end
