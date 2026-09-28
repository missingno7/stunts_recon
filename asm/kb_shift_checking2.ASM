.8086
INCLUDE include\platform_hw.inc
extrn _kb_checking:far
S018_TEXT segment byte public 'CODE'
assume cs:S018_TEXT
public _kb_shift_checking2
; -----------------------------------------------------------------------------
; Purpose: Implement the keyboard or joystick service identified by kb shift checking2.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_kb_shift_checking2 proc far
 mov ax,40h
 mov es,ax
 and byte ptr es:[17h],0DFh
 call far ptr _kb_checking
 ret
_kb_shift_checking2 endp
S018_TEXT ends
end
