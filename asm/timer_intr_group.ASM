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
