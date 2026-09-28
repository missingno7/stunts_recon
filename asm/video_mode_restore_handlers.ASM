.8086
INCLUDE include\platform_hw.inc
name asm012_140718
_DATA segment word public 'DATA'
_currentvideomode db 0
saved_video_control_register db 0
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
assume ds:DGROUP
extrn _add_exit_handler:far
extrn _video_clear_color:far
public _video_add_exithandler
public _video_on_exit
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP

; video_add_exithandler
assume cs:S012_TEXT, ds:DGROUP
; -----------------------------------------------------------------------------
; Purpose: Register a video cleanup handler for process exit.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, SP, CS, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _currentvideomode.
; Globals written: _currentvideomode, saved_video_control_register.
; /* PLATFORM(video): calls a BIOS video service. */
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_video_add_exithandler proc far
    cmp byte ptr ds:[_currentvideomode], 0
    jne skip_if_saved_mode_exists
    mov ah, 0Fh
    int R5HW_BIOS_VIDEO_INT ;/* PLATFORM(video): calls a BIOS video service. */
    mov byte ptr ds:[_currentvideomode], al
    mov ax, 40h
    mov es, ax
    mov al, byte ptr es:[10h] ;/* PLATFORM(video): accesses display memory through ES. */
    mov byte ptr ds:[saved_video_control_register], al
    push cs
    mov ax, offset _video_on_exit
    push ax
    call far ptr _add_exit_handler
    add sp, 4
skip_if_saved_mode_exists:
    retf
_video_add_exithandler endp

; video_on_exit
assume cs:S012_TEXT, ds:DGROUP
; -----------------------------------------------------------------------------
; Purpose: Restore the saved video mode during process cleanup.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, SP, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _currentvideomode, saved_video_control_register.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; /* PLATFORM(video): calls a BIOS video service. */
; -----------------------------------------------------------------------------
_video_on_exit proc far
    mov ax, 40h
    mov es, ax
    mov al, byte ptr ds:[saved_video_control_register]
    mov byte ptr es:[10h], al ;/* PLATFORM(video): accesses display memory through ES. */
    mov ah, 0
    mov al, byte ptr ds:[_currentvideomode]
    int R5HW_BIOS_VIDEO_INT ;/* PLATFORM(video): calls a BIOS video service. */
    mov ax, 40h
    mov es, ax
    mov al, byte ptr ds:[saved_video_control_register]
    mov byte ptr es:[10h], al ;/* PLATFORM(video): accesses display memory through ES. */
    and al, 30h
    cmp al, 30h
    jne skip_color_clear_for_non_cga
    xor ax, ax
    push ax
    call far ptr _video_clear_color
    add sp, 2
skip_color_clear_for_non_cga:
    mov ah, 0Bh
    mov bx, 0
    int R5HW_BIOS_VIDEO_INT ;/* PLATFORM(video): calls a BIOS video service. */
    retf
_video_on_exit endp

S012_TEXT ends
end
