.model medium
DGROUP group dseg
dseg segment byte public 'STUNTSD'
assume ds:dseg
extrn _word_3F87C:word
extrn _word_3F87E:word
dseg ends
_TEXT segment word public 'CODE'
assume cs:_TEXT, ds:dseg
public _sub_2EAD4
_sub_2EAD4 proc far

    cli
    mov     ax, _word_3F87C
    mov     dx, _word_3F87E
timer_counter_atomic_finish:
    sti
counter_atomic_early_return:
    retf
_sub_2EAD4 endp
_TEXT ends
end
