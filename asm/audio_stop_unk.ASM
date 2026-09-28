.model medium
DGROUP group dseg
dseg segment byte public 'STUNTSD'
assume ds:dseg
extrn _dword_3F874:dword
dseg ends
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:dseg
public _audio_stop_unknown
_audio_stop_unknown proc far

    xor     ax, ax
    mov     es, ax
    mov     ax, es:22h
    mov     dx, cs
    cmp     ax, dx
    jz      short stop_unknown_return_if_zero
    retf
stop_unknown_return_if_zero:
    mov     ax, es:20h
    cmp     ax, 1909h
    jz      short audio_stop_unknown_when_zero
    retf
audio_stop_unknown_when_zero:
    in      al, 21h         ; Interrupt controller, 8259A.
    or      al, 3
    out     21h, al         ; Interrupt controller, 8259A.
    cli
    mov     ax, word ptr _dword_3F874
    mov     es:20h, ax
    mov     ax, word ptr _dword_3F874+2
    mov     es:22h, ax
    in      al, 21h         ; Interrupt controller, 8259A.

    and     al, 0FCh

    out     21h, al         ; Interrupt controller, 8259A.
    sti
    mov     al, 0
    out     40h, al         ; Timer 8253-5 (AT: 8254.2).
    out     40h, al         ; Timer 8253-5 (AT: 8254.2).
    in      al, 61h         ; PC/XT PPI port B bits:

    and     al, 0FCh

    out     61h, al         ; PC/XT PPI port B bits:
    retf
_audio_stop_unknown endp
S012_TEXT ends
end
