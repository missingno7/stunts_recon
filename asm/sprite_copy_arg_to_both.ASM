.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _sprite1:byte
public _sprite_copy_arg_to_both
; -----------------------------------------------------------------------------
; Purpose: Copy the supplied sprite image into both sprite buffers.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, CX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _sprite1.
; -----------------------------------------------------------------------------
_sprite_copy_arg_to_both proc far
 push bp
 mov bp,sp
 push ds
 push di
 push si
 mov si,word ptr [bp+6]
 mov ax,cs
 mov es,ax
 lea di,_sprite1
 cld
 mov cx,1Eh
 rep movsw
 pop si
 pop di
 pop ds
 pop bp
 ret
_sprite_copy_arg_to_both endp
S012_TEXT ends
end
