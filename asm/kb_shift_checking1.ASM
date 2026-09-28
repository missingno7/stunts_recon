.8086
INCLUDE include\platform_hw.inc
extrn _kb_checking:far
S018_TEXT segment word public 'CODE'
assume cs:S018_TEXT
public _keyboard_shift_checking1
; -----------------------------------------------------------------------------
; Purpose: Implement the keyboard or joystick service identified by keyboard shift checking1.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_keyboard_shift_checking1 proc far
 mov ax,40h
 mov es,ax
 or byte ptr es:[17h],20h
 call far ptr _kb_checking
 ret
_keyboard_shift_checking1 endp
S018_TEXT ends
end
