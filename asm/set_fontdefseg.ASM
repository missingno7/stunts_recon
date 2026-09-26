.8086
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
_TEXT segment word public 'CODE'
assume cs:_TEXT,ds:DGROUP
extrn _fontdefseg:word
public _set_fontdefseg
_set_fontdefseg proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_2]
    mov     ds:[_fontdefseg], ax
    pop     bp
    retf
    ; align 2
    db 0
_set_fontdefseg endp
_TEXT ends
end
