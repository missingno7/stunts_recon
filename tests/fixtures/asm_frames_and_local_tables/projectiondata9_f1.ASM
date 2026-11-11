.8086
_TEXT segment word public 'CODE'
_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:_TEXT, ds:DGROUP
extrn _word_303BA:word
_TEXT segment word public 'CODE'
public _projectiondata9_times_ratio
_projectiondata9_times_ratio proc far
    push bp
    mov bp, sp
    mov ax, word ptr DGROUP:_word_303BA
    mul word ptr [bp+6]
    div word ptr [bp+8]
    pop bp
    retf
_projectiondata9_times_ratio endp
_TEXT ends
end
