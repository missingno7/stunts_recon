.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _byte_3F880:byte
extrn _byte_3F881:byte
extrn _word_3F882:word
extrn _dword_3F874:dword
DSEG ends
_TEXT segment word public 'CODE'
assume cs:_TEXT, ds:DSEG
public _sub_303BA
_sub_303BA proc near

    cmp     _byte_3F880, 0
    jz      short loc_303D1
    dec     _word_3F882
    jg      short loc_303D1
    mov     _byte_3F880, 0
    mov     _byte_3F881, 0
loc_303D1:
    pushf
    call    _dword_3F874
    retn
    ; align 2
    db 0
_sub_303BA endp
_TEXT ends
end
