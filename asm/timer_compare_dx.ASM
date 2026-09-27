.8086
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
extrn _timer_copy_unk:word
extrn _word_305F8:word
S012_TEXT segment byte public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
extrn _timer_get_counter:far
public _timer_compare_dx
_timer_compare_dx proc far
 call far ptr _timer_get_counter
 cmp dx,word ptr ds:[_word_305F8]
 jb zero_result
 ja one_result
 cmp ax,word ptr ds:[_timer_copy_unk]
 jb zero_result
one_result:
 mov ax,1
 ret
zero_result:
 xor ax,ax
 ret
_timer_compare_dx endp
S012_TEXT ends
end
