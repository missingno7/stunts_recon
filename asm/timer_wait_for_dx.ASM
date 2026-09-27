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
extrn _timer_copy_unk:word
extrn _word_305F8:word
extrn _timer_get_counter:far
S012_TEXT segment byte public 'CODE'
public _timer_wait_for_dx
_timer_wait_for_dx proc far
    call    _timer_get_counter
    cmp     dx, word ptr _word_305F8
    jb      short near ptr _timer_wait_for_dx
    ja      short locret_327EA
    cmp     ax, word ptr _timer_copy_unk
    jb      short near ptr _timer_wait_for_dx
locret_327EA:
    retf
_timer_wait_for_dx endp
S012_TEXT ends
end
