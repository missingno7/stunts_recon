.8086
extrn _kb_checking:far
S018_TEXT segment byte public 'CODE'
assume cs:S018_TEXT
public _kb_shift_checking2
_kb_shift_checking2 proc far
 mov ax,40h
 mov es,ax
 and byte ptr es:[17h],0DFh
 call far ptr _kb_checking
 ret
_kb_shift_checking2 endp
S018_TEXT ends
end
