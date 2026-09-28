.8086
.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _word_3F87C:word
extrn _word_3F87E:word
extrn _word_3F1C2:word
extrn _word_3F1C4:word
DSEG ends
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _sub_2EAD4, _set_add_value, _sub_2EB07, _sub_2EB1E
_sub_2EAD4 proc far
    cli
    mov     ax, _word_3F87C
    mov     dx, _word_3F87E
enable_counter_read:
    sti
return_timer_counter:
    retf

_sub_2EAD4 endp
_set_add_value proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
prepare_deadline_frame:
    mov     bp, sp
read_current_timer_counter:
    call    _sub_2EAD4
add_deadline_low_word:
    add     ax, [bp+arg_0]
add_deadline_high_word:
    adc     dx, [bp+arg_2]
store_deadline_low_word:
    mov     _word_3F1C2, ax
store_deadline_high_word:
    mov     _word_3F1C4, dx
    pop     bp
    retf
wait_for_timer_deadline:
    call    _sub_2EAD4
    cmp     dx, _word_3F1C4
    jb      short wait_for_timer_deadline
    cmp     ax, _word_3F1C2
    jb      short wait_for_timer_deadline
    retf

_set_add_value endp
_sub_2EB07 proc far
    call    _sub_2EAD4
    xor     cx, cx
    cmp     dx, _word_3F1C4
    jb      short return_deadline_reached
    cmp     ax, _word_3F1C2
    jb      short return_deadline_reached
    inc     cx
return_deadline_reached:
    mov     ax, cx
    retf

_sub_2EB07 endp
_sub_2EB1E proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 4
    call    _sub_2EAD4
    add     ax, [bp+arg_0]
    adc     dx, [bp+arg_2]
    mov     [bp+var_4], ax
    mov     [bp+var_2], dx
poll_until_timer_deadline:
    call    _sub_2EAD4
    cmp     dx, [bp+var_2]
    jb      short poll_until_timer_deadline
    cmp     ax, [bp+var_4]
    jb      short poll_until_timer_deadline
    mov     sp, bp
    pop     bp
    retf

_sub_2EB1E endp
_TEXT ends
end
