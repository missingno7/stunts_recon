.model medium
DGROUP group dseg
dseg segment byte public 'STUNTSD'
assume ds:dseg
extrn _byte_3FB38:byte
dseg ends
_TEXT segment word public 'CODE'
assume cs:_TEXT, ds:dseg
public _sub_307D2
_sub_307D2 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     bx, [bp+arg_0]

    and     bx, 0Fh

    mov     al, _byte_3FB38[bx]
    xor     ah, ah
    pop     bp
    retf
_sub_307D2 endp
_TEXT ends
end
