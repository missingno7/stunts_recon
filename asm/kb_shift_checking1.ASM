.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn kb_shift_state:far
public _kb_shift_checking1
_kb_shift_checking1 proc far
 mov ax,40h
 mov es,ax
 or byte ptr es:[17h],20h
 call far ptr kb_shift_state
 ret
_kb_shift_checking1 endp
_TEXT ends
end
