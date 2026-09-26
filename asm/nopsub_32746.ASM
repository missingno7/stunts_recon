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
public _nopsub_32746
_nopsub_32746 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, _word_303BA
    mul     [bp+arg_0]
    pop     bp
    retf
_nopsub_32746 endp
_TEXT ends
end
