.8086
S012_TEXT segment word public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:DGROUP
extrn _byte_4032C:byte
S012_TEXT segment word public 'CODE'
public _nopsub_322B4
_nopsub_322B4 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_0]
    mov     _byte_4032C, al
    pop     bp
    retf

    db 0
_nopsub_322B4 endp
S012_TEXT ends
end
