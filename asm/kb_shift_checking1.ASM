.8086
extrn _kb_checking:far
S018_TEXT segment word public 'CODE'
assume cs:S018_TEXT
public _kb_shift_checking1
_kb_shift_checking1 proc far
 mov ax,40h
 mov es,ax
 or byte ptr es:[17h],20h
 call far ptr _kb_checking
 ret
_kb_shift_checking1 endp
S018_TEXT ends
end
