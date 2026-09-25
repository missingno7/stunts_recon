.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn timer_get_counter:far
public _timer_compare_dx
_timer_compare_dx proc far
 call far ptr timer_get_counter
 cmp dx,word ptr ds:[4E88h]
 jb zero_result
 ja one_result
 cmp ax,word ptr ds:[4E86h]
 jb zero_result
one_result:
 mov ax,1
 ret
zero_result:
 xor ax,ax
 ret
_timer_compare_dx endp
_TEXT ends
end
