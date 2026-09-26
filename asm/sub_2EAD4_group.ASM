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
loc_2EADC:
    sti
locret_2EADD:
    retf

_sub_2EAD4 endp
_set_add_value proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
loc_2EADF:
    mov     bp, sp
loc_2EAE1:
    call    _sub_2EAD4
loc_2EAE6:
    add     ax, [bp+arg_0]
loc_2EAE9:
    adc     dx, [bp+arg_2]
loc_2EAEC:
    mov     _word_3F1C2, ax
loc_2EAEF:
    mov     _word_3F1C4, dx
    pop     bp
    retf
loc_2EAF5:
    call    _sub_2EAD4
    cmp     dx, _word_3F1C4
    jb      short loc_2EAF5
    cmp     ax, _word_3F1C2
    jb      short loc_2EAF5
    retf

_set_add_value endp
_sub_2EB07 proc far
    call    _sub_2EAD4
    xor     cx, cx
    cmp     dx, _word_3F1C4
    jb      short loc_2EB1B
    cmp     ax, _word_3F1C2
    jb      short loc_2EB1B
    inc     cx
loc_2EB1B:
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
loc_2EB35:
    call    _sub_2EAD4
    cmp     dx, [bp+var_2]
    jb      short loc_2EB35
    cmp     ax, [bp+var_4]
    jb      short loc_2EB35
    mov     sp, bp
    pop     bp
    retf

_sub_2EB1E endp
_TEXT ends
end
