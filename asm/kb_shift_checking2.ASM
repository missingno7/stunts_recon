.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn kb_shift_state:far
public _kb_shift_checking2
_kb_shift_checking2 proc far
 mov ax,40h
 mov es,ax
 and byte ptr es:[17h],0DFh
 call far ptr kb_shift_state
 ret
_kb_shift_checking2 endp
_TEXT ends
end
