.8086
INCLUDE include\platform_hw.inc
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
public _timer_callback_busy, _input_pushed, _timerintr, _aNoRoomLeftOnTimerInterru
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
_input_pushed dw 0
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
; -----------------------------------------------------------------------------
; Purpose: Return the 2D polar radius for the supplied point.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_y (word ptr 8); arg_z (word ptr 6).
; Register ABI: touches AX, BX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
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
; -----------------------------------------------------------------------------
; Purpose: Configure the BIOS mode-7 text display.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, DI, DS, ES; balanced explicit saves/restores observed: DI; path-specific inputs and outputs follow the listed operands.
; Globals read: _video_mode4_active.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; /* PLATFORM(video): accesses a VGA/MCGA display register port. */
; /* PLATFORM(video): selects a video-memory segment. */
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; /* PLATFORM(video): calls a BIOS video service. */
; -----------------------------------------------------------------------------
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
    mov ax, word ptr es:[10h] ;/* PLATFORM(video): accesses display memory through ES. */
    and ax, 0FFCFh
    or ax, 30h
    mov word ptr es:[10h], ax ;/* PLATFORM(video): accesses display memory through ES. */
    mov al, 3
    mov dx, R5HW_VIDEO_MONO_MISC_PORT
    out dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    mov al, 20h
    mov dx, R5HW_VIDEO_MONO_MODE_PORT
    out dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    mov cx, 0Ch
    mov dx, R5HW_VIDEO_MONO_CRTC_INDEX_PORT
    xor bx, bx
mode7_write_crtc_register_loop:
    mov al, bl
    out dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    inc dx
    mov al, byte ptr [bx + 40F8h]
    out dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    dec dx
    inc bx
    loop mode7_write_crtc_register_loop
    cld
    mov ax, R5HW_VIDEO_TEXT_SEGMENT ;/* PLATFORM(video): selects a video-memory segment. */
    mov es, ax
    xor di, di
    mov cx, 4000h
    xor ax, ax
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    mov al, 28h
    mov dx, R5HW_VIDEO_MONO_MODE_PORT
    out dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    mov ah, 0
    mov al, 7
    int R5HW_BIOS_VIDEO_INT ;/* PLATFORM(video): calls a BIOS video service. */
    pop di
    retf
_video_set_mode7 endp

; nopsub_30180: _nopsub_30180
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 30180.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _bios_tick_countdown, _timer_interrupt_reload_value, _timer_sound_countdown_active, _timer_sound_countdown_ticks.
; -----------------------------------------------------------------------------
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
; -----------------------------------------------------------------------------
; Purpose: Install and configure the game timer interrupt handler.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, DX, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _bios_tick_countdown, _previous_timer_irq_vector, _timer_callback_busy, _timer_interrupt_reload_value, _timer_sound_countdown_active,
;   _timer_sound_countdown_enabled, _timer_sound_countdown_ticks, _timerintr.
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
; /* PLATFORM(timer): accesses the programmable interval timer port. */
; -----------------------------------------------------------------------------
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
 cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
 mov byte ptr ds:[_timer_callback_busy],0
 mov word ptr ds:[_timerintr],0
 mov word ptr ds:[_timerintr+2],0
 sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
 in al,R5HW_KEYBOARD_CONTROL_PORT ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
 and al,0FCh
 out R5HW_KEYBOARD_CONTROL_PORT,al ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
 mov al,0B6h
 out R5HW_PIT_CONTROL_PORT,al ;/* PLATFORM(timer): accesses the programmable interval timer port. */
 in al,R5HW_PIC_MASTER_MASK_PORT ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
 or al,3
 out R5HW_PIC_MASTER_MASK_PORT,al ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
 xor ax,ax
 mov es,ax
 cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
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
 in al,R5HW_PIC_MASTER_MASK_PORT ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
 and al,0FCh
 out R5HW_PIC_MASTER_MASK_PORT,al ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
 sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
 mov al,dl
 out R5HW_PIT_CHANNEL0_PORT,al ;/* PLATFORM(timer): accesses the programmable interval timer port. */
 mov al,dh
 out R5HW_PIT_CHANNEL0_PORT,al ;/* PLATFORM(timer): accesses the programmable interval timer port. */
 push cs
 mov ax,1848h
 push ax
 call far ptr _add_exit_handler
 add sp,4
 pop bp
 ret

_timer_setup_interrupt endp

; audio_stop_unk: _audio_stop_unknown
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by audio stop unknown.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, DX, CS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _previous_timer_irq_vector.
; Globals written: none identified.
; /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; /* PLATFORM(timer): accesses the programmable interval timer port. */
; -----------------------------------------------------------------------------
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
    in      al, R5HW_PIC_MASTER_MASK_PORT ; Interrupt controller, 8259A. /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
    or      al, 3
    out     R5HW_PIC_MASTER_MASK_PORT, al ; Interrupt controller, 8259A. /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov     ax, word ptr _previous_timer_irq_vector
    mov     es:20h, ax
    mov     ax, word ptr _previous_timer_irq_vector+2
    mov     es:22h, ax
    in      al, R5HW_PIC_MASTER_MASK_PORT ; Interrupt controller, 8259A. /* PLATFORM(dos): accesses the PIC or keyboard controller port. */

    and     al, 0FCh

    out     R5HW_PIC_MASTER_MASK_PORT, al ; Interrupt controller, 8259A. /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov     al, 0
    out     R5HW_PIT_CHANNEL0_PORT, al ; Timer 8253-5 (AT: 8254.2). /* PLATFORM(timer): accesses the programmable interval timer port. */
    out     R5HW_PIT_CHANNEL0_PORT, al ; Timer 8253-5 (AT: 8254.2). /* PLATFORM(timer): accesses the programmable interval timer port. */
    in      al, R5HW_KEYBOARD_CONTROL_PORT ; PC/XT PPI port B bits: /* PLATFORM(dos): accesses the PIC or keyboard controller port. */

    and     al, 0FCh

    out     R5HW_KEYBOARD_CONTROL_PORT, al ; PC/XT PPI port B bits: /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
    retf
_audio_stop_unknown endp

; timer_reg_callback: _timer_reg_callback
; -----------------------------------------------------------------------------
; Purpose: Register a timer callback in the callback table.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8).
; Register ABI: touches AX, BX, CX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _aNoRoomLeftOnTimerInterru, _timerintr.
; -----------------------------------------------------------------------------
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
; -----------------------------------------------------------------------------
; Purpose: Remove a timer callback from the callback table.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _timerintr.
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; -----------------------------------------------------------------------------
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
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
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
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    pop     bp
    retf
_timer_remove_callback endp

; compare_ds_ss: _compare_ds_ss
; -----------------------------------------------------------------------------
; Purpose: Implement the timer service identified by compare ds ss.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, DX, DS, SS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
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
; -----------------------------------------------------------------------------
; Purpose: Dispatch the timer interrupt callback and acknowledge the interrupt.
; ABI: FAR entry; source return form: IRET; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, DS, ES; balanced explicit saves/restores observed: AX, BX, CX, DI, DS, DX, ES, SI; path-specific inputs and outputs follow
;   the listed operands.
; Globals read: _bios_tick_countdown, _timer_callback_busy, _timer_callback_counter, _timer_callback_reentry_depth, _timer_callback_reentry_peak, _timer_elapsed_ticks_high,
;   _timer_elapsed_ticks_low, _timer_interrupt_reload_value, _timer_sound_countdown_enabled, _timerintr, _input_pushed.
; Globals written: _bios_tick_countdown, _timer_callback_busy, _timer_callback_counter, _timer_callback_reentry_depth, _timer_callback_reentry_peak, _timer_elapsed_ticks_high,
;   _timer_elapsed_ticks_low.
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
; /* PLATFORM(timer): returns from a hardware timer interrupt. */
; -----------------------------------------------------------------------------
_timer_intr_callback proc far
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
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
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
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
    out R5HW_PIC_MASTER_COMMAND_PORT, al ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
timer_check_callback_enable:
    cmp byte ptr _input_pushed, 0
    jne timer_clear_reentry_state
    inc word ptr _timer_callback_counter
    jne timer_advance_callback_clock
    inc word ptr _timer_callback_counter+2
timer_advance_callback_clock:
    xor di, di
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    cmp byte ptr _timer_callback_busy, 0
    jne timer_count_reentrant_tick
    mov byte ptr _timer_callback_busy, 1
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
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
    iret ;/* PLATFORM(timer): returns from a hardware timer interrupt. */
_timer_intr_callback endp

; timer_intr_group: _sub_303BA
; -----------------------------------------------------------------------------
; Purpose: Advance the timer sound countdown and chain to the previous timer interrupt.
; ABI: NEAR entry; source return form: RET mnemonic for NEAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _timer_sound_countdown_active, _timer_sound_countdown_ticks.
; Globals written: _timer_sound_countdown_active, _timer_sound_countdown_enabled, _timer_sound_countdown_ticks.
; -----------------------------------------------------------------------------
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
