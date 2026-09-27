.8086
S012_TEXT segment byte public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:DGROUP
extrn _word_303BC:word
S012_TEXT segment byte public 'CODE'
public _nopsub_32751
_nopsub_32751 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, _word_303BC
    mul     [bp+arg_0]
    pop     bp
    retf
_nopsub_32751 endp
S012_TEXT ends
end
