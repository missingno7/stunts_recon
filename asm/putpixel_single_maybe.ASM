.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _putpixel_single_maybe
; -----------------------------------------------------------------------------
; Purpose: Write one pixel to the selected video page.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, SI, DI, BP, SP, CS, ES; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_putpixel_single_maybe proc far
 push bp
 mov bp,sp
 push si
 push di
 mov ax,word ptr [bp+6]
 cmp ax,word ptr cs:[_sprite1+12]
 jl short reject_pixel
 cmp ax,word ptr cs:[_sprite1+14]
 jge short reject_pixel
 mov ax,word ptr [bp+8]
 cmp ax,word ptr cs:[_sprite1+16]
 jl short reject_pixel
 cmp ax,word ptr cs:[_sprite1+18]
 jl short write_pixel
reject_pixel:
 pop di
 pop si
 pop bp
 ret
 push bp
 mov bp,sp
 push si
 push di
write_pixel:
 mov di,word ptr [bp+6]
 mov si,word ptr [bp+8]
 shl si,1
 add si,word ptr cs:[_sprite1+10]
 add di,word ptr cs:[si]
 mov es,word ptr cs:[_sprite1+2]
 mov ax,word ptr [bp+0Ah]
 mov byte ptr es:[di],al ;/* PLATFORM(video): accesses display memory through ES. */
 pop di
 pop si
 pop bp
 ret
 db 0
_putpixel_single_maybe endp
S012_TEXT ends
end
