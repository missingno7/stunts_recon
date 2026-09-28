.8086
DGROUP group DSEG, _DATA
DSEG segment byte public 'STUNTSD'
DSEG ends
assume ds:DGROUP
_DATA segment word public 'DATA'
public _byte_3F868, _dword_3F874, _timer_callback_counter
public _word_3F87C, _word_3F87E, _byte_3F880, _byte_3F881
public _word_3F882, _word_3F884, _word_3F886, _word_3F888, _word_3F88A
public _byte_3F88C, _word_3F88E, _timerintr, _aNoRoomLeftOnTimerInterru
_byte_3F868 db 97, 80, 82, 15, 25, 6, 25, 25, 2, 13, 11, 12
_dword_3F874 dd 0
_timer_callback_counter dd 0
_word_3F87C dw 0
_word_3F87E dw 0
_byte_3F880 db 0
_byte_3F881 db 0
_word_3F882 dw 0
_word_3F884 dw 0
_word_3F886 dw 0
_word_3F888 dw 0
_word_3F88A dw 0
_byte_3F88C db 0
db 3
_word_3F88E dw 0
_timerintr dd 0
db 24 dup (0)
_aNoRoomLeftOnTimerInterru db "NO ROOM LEFT ON TIMER INTERRUPT ROUTINE LIST", 13, 0
_DATA ends
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _polradius2d, _video_set_mode7, _nopsub_30180, _timer_setup_interrupt, _audio_stop_unknown, _timer_reg_callback, _timer_remove_callback, _compare_ds_ss, _timer_intr_callback, _sub_303BA
extrn _cosfast:far
extrn _polang:far
extrn _sinfast:far
extrn _byte_3F85A:byte
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
    jge short loc_200CD
    neg ax
loc_200CD:
    cmp ax, 100h
    jl short loc_200D7
    sub ax, 200h
    neg ax
loc_200D7:
    cmp ax, 80h
    jg short loc_200FE
    push ax
    call far ptr _cosfast
    add sp, 2
    mov bx, ax
    mov dx, word ptr [bp+arg_y]
    or dx, dx
    jge short loc_200F0
    neg dx
loc_200F0:
    xor ax, ax
    sar dx, 1
    rcr ax, 1
    sar dx, 1
    rcr ax, 1
    div bx
    pop bp
    retf
loc_200FE:
    push ax
    call far ptr _sinfast
    add sp, 2
    mov bx, ax
    mov dx, word ptr [bp+arg_z]
    or dx, dx
    jge short loc_20112
    neg dx
loc_20112:
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
    cmp byte ptr ds:[_byte_3F85A], 0
    jne loc_2012F
    call far ptr _set_bios_mode3
    pop di
    retf
loc_2012F:
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
loc_20156:
    mov al, bl
    out dx, al
    inc dx
    mov al, byte ptr [bx + 40F8h]
    out dx, al
    dec dx
    inc bx
    loop loc_20156
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
    mov     _word_3F884, 5
    mov     _word_3F886, 5
    mov     _word_3F882, 64h ; 'd'
    mov     _byte_3F880, 1
    jmp     short shared_setup
    ; align 2
    db 144

_nopsub_30180 endp

; nopsub_30180: _timer_setup_interrupt
_timer_setup_interrupt proc far
 push bp
 mov dx,2E9Ch
 mov word ptr ds:[_word_3F884],5
 mov word ptr ds:[_word_3F886],5
 mov byte ptr ds:[_byte_3F880],0
 mov byte ptr ds:[_byte_3F881],1
 jmp shared_setup
 push bp
 mov bp,sp
 mov dx,1
 xor ax,ax
 idiv word ptr [bp+6]
 mov word ptr ds:[_word_3F884],ax
 mov word ptr ds:[_word_3F886],ax
 mov dx,word ptr [bp+6]
 mov byte ptr ds:[_byte_3F880],0
 mov byte ptr ds:[_byte_3F881],1
 jmp shared_setup
 push bp
 mov bp,sp
 mov dx,1
 xor ax,ax
 idiv word ptr [bp+6]
 mov word ptr ds:[_word_3F884],ax
 mov word ptr ds:[_word_3F886],ax
 mov dx,word ptr [bp+6]
 mov word ptr ds:[_word_3F882],64h
 mov byte ptr ds:[_byte_3F880],1
shared_setup:
 cli
 mov byte ptr ds:[_byte_3F88C],0
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
 mov word ptr ds:[_dword_3F874],ax
setup_after_old_off:
 mov ax,word ptr es:[22h]
 mov bx,cs
 cmp ax,bx
 je setup_after_old_seg
 mov word ptr ds:[_dword_3F874+2],ax
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
    jz      short loc_30277
    retf
loc_30277:
    mov     ax, es:20h
    cmp     ax, 1909h
    jz      short loc_30281
    retf
loc_30281:
    in      al, 21h         ; Interrupt controller, 8259A.
    or      al, 3
    out     21h, al         ; Interrupt controller, 8259A.
    cli
    mov     ax, word ptr _dword_3F874
    mov     es:20h, ax
    mov     ax, word ptr _dword_3F874+2
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
loc_302B3:
    cmp     word ptr [bx+2], 0
    jz      short loc_302C7
    add     bx, 4
    loop    loc_302B3
    mov     ax, offset DGROUP:_aNoRoomLeftOnTimerInterru; "NO ROOM LEFT ON TIMER INTERRUPT ROUTINE"...
    push    ax
    call    far ptr _fatal_error
loc_302C7:
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
loc_302ED:
    cmp     [bx], ax
    jnz     short loc_302F6
    cmp     [bx+2], dx
    jz      short loc_302FD
loc_302F6:
    add     bx, 4
    loop    loc_302ED
    pop     bp
    retf
loc_302FD:
    cli
    dec     cx
    jz      short loc_30311
loc_30301:
    mov     ax, [bx+4]
    mov     [bx], ax
    mov     ax, [bx+6]
    mov     [bx+2], ax
    add     bx, 4
    loop    loc_30301
loc_30311:
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
    jnz     short locret_30328
    inc     ax
locret_30328:
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
    dec word ptr _word_3F886
    jg loc_2035C
    inc word ptr _word_3F87C
    jne loc_20349
    inc word ptr _word_3F87E
loc_20349:
    mov ax, word ptr _word_3F884
    mov word ptr _word_3F886, ax
    cmp byte ptr _byte_3F881, 0
    je loc_2035C
    call loc_203BA
    jmp short loc_20360
    nop
loc_2035C:
    mov al, 20h
    out 20h, al
loc_20360:
    cmp byte ptr _word_3F88E, 0
    jne loc_203A5
    inc word ptr _timer_callback_counter
    jne loc_20371
    inc word ptr _timer_callback_counter+2
loc_20371:
    xor di, di
    cli
    cmp byte ptr _byte_3F88C, 0
    jne loc_20392
    mov byte ptr _byte_3F88C, 1
    sti
loc_20381:
    mov ax, word ptr _timerintr[di+2]
    or ax, ax
    je loc_203A5
    call dword ptr _timerintr[di]
    add di, 4
    jmp loc_20381
loc_20392:
    inc word ptr _word_3F88A
    mov ax, word ptr _word_3F88A
    cmp word ptr _word_3F888, ax
    jge loc_203B0
    mov word ptr _word_3F888, ax
    jmp short loc_203B0
    nop
loc_203A5:
    mov byte ptr _byte_3F88C, 0
    mov word ptr _word_3F88A, 0
loc_203B0:
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
loc_203BA:
    cmp byte ptr _byte_3F880, 0
    je loc_203D1
    dec word ptr _word_3F882
    jg loc_203D1
    mov byte ptr _byte_3F880, 0
    mov byte ptr _byte_3F881, 0
loc_203D1:
    pushf
    call dword ptr _dword_3F874
    retn
    db 0 ; single verified trailing alignment/data byte
_sub_303BA endp
S012_TEXT ends
end
