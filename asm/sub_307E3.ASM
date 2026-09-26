.model medium
DGROUP group dseg
dseg segment byte public 'STUNTSD'
assume ds:dseg
extrn _joyflag1:word
extrn _word_3FB18:word
extrn _word_3FB34:word
dseg ends
_TEXT segment word public 'CODE'
assume cs:_TEXT, ds:dseg
public _sub_307E3
_sub_307E3 proc far

    mov     ax, _joyflag1
    sub     ax, _word_3FB18
    jge     short loc_307EE
    xor     ax, ax
loc_307EE:
    mul     _word_3FB34
    mov     al, ah
    mov     ah, dl
    sub     ax, 1Fh
    retf
_sub_307E3 endp
_TEXT ends
end
