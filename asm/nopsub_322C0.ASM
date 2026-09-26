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
extrn _word_303AE:word
extrn _word_303B2:word
extrn _word_303B6:word
extrn _word_303B0:word
extrn _word_303B4:word
extrn _word_303B8:word
_TEXT segment word public 'CODE'
public _nopsub_322C0
_nopsub_322C0 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_0]
    mov     _word_303B2, ax
    add     ax, _word_303AE
    mov     _word_303B6, ax
    mov     ax, [bp+arg_2]
    mov     _word_303B4, ax
    add     ax, _word_303B0
    mov     _word_303B8, ax
    pop     bp
    retf
_nopsub_322C0 endp
_TEXT ends
end
