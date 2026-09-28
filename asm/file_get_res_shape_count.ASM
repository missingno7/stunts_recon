.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _file_get_res_shape_count
; -----------------------------------------------------------------------------
; Purpose: Read the shape count from a resource record.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, SI, BP, SP, ES; balanced explicit saves/restores observed: SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_file_get_res_shape_count proc far
 push bp
 mov bp,sp
 push si
 mov es,[bp+8]
 mov si,word ptr [bp+6]
 mov ax,word ptr es:[si+4] ;/* PLATFORM(video): accesses display memory through ES. */
 pop si
 pop bp
 ret
_file_get_res_shape_count endp
S012_TEXT ends
end
