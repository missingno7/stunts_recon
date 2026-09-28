.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment byte public 'CODE'
assume cs:S012_TEXT
public _file_get_shape2d
; -----------------------------------------------------------------------------
; Purpose: Resolve a shape index to the corresponding 2D shape data.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, SI, BP, SP, ES; balanced explicit saves/restores observed: SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_file_get_shape2d proc far
 push bp
 mov bp,sp
 push si
 mov es,[bp+8]
 mov si,word ptr [bp+6]
 mov ax,word ptr es:[si+4] ;/* PLATFORM(video): accesses display memory through ES. */
 shl ax,1
 shl ax,1
 mov bx,word ptr [bp+10]
 shl bx,1
 shl bx,1
 add bx,ax
 add bx,6
 shl ax,1
 add ax,6
 mov cx,es
 xor dx,dx
 shl cx,1
 rcl dx,1
 shl cx,1
 rcl dx,1
 shl cx,1
 rcl dx,1
 shl cx,1
 rcl dx,1
 add ax,cx
 adc dx,0
 add ax,word ptr es:[bx] ;/* PLATFORM(video): accesses display memory through ES. */
 adc dx,word ptr es:[bx+2] ;/* PLATFORM(video): accesses display memory through ES. */
 mov bx,ax
 and bx,0Fh
 shr dx,1
 rcr ax,1
 shr dx,1
 rcr ax,1
 shr dx,1
 rcr ax,1
 shr dx,1
 rcr ax,1
 mov dx,ax
 mov ax,bx
 pop si
 pop bp
 ret
_file_get_shape2d endp
S012_TEXT ends
end
