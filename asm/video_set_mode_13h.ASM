.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _video_clear_color:far
extrn _video_add_exithandler:far
public _video_set_mode_13h
; -----------------------------------------------------------------------------
; Purpose: Enter MCGA mode 13h and install the associated cleanup handlers.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, SP, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; /* PLATFORM(video): calls a BIOS video service. */
; -----------------------------------------------------------------------------
_video_set_mode_13h proc far
 call far ptr _video_add_exithandler
 mov ax,40h
 mov es,ax
 mov ax,word ptr es:[10h] ;/* PLATFORM(video): accesses display memory through ES. */
 and ax,0FFCFh
 or ax,10h
 mov word ptr es:[10h],ax ;/* PLATFORM(video): accesses display memory through ES. */
 mov ah,0Bh
 mov bx,0
 int R5HW_BIOS_VIDEO_INT ;/* PLATFORM(video): calls a BIOS video service. */
 mov ah,0
 mov al,13h
 int R5HW_BIOS_VIDEO_INT ;/* PLATFORM(video): calls a BIOS video service. */
 xor ax,ax
 push ax
 push ax
 call far ptr _video_clear_color
 add sp,4
 ret
_video_set_mode_13h endp
S012_TEXT ends
end
