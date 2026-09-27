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
extrn _timer_copy_unk:word
extrn _word_305F8:word
extrn _timer_get_counter:far
S012_TEXT segment word public 'CODE'
public _timer_copy_counter
_timer_copy_counter proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_ticksl = word ptr 6
    arg_ticksh = word ptr 8

    push    bp
    mov     bp, sp
    call    _timer_get_counter
    add     ax, [bp+arg_ticksl]
    adc     dx, [bp+arg_ticksh]
    mov     word ptr _timer_copy_unk, ax
    mov     word ptr _word_305F8, dx
    pop     bp
    retf
_timer_copy_counter endp
S012_TEXT ends
end
