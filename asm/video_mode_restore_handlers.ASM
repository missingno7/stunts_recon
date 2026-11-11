; ASMDOC[timers] Module: Capture BIOS display mode and low equipment byte once, register an exit callback, then
; ASMDOC[timers]   restore that mode and equipment byte. BDA equipment flags are not a video control port.
; ASMDOC[timers] Authoritative contracts below refine older retained comments; original source bytes are untouched.
; ASMDOC[timers] Stack offsets BP+6/BP+8 refer to saved-BP FAR frame; entry SP+4/SP+6 refer to original FAR frame.
.8086
INCLUDE include\platform_hw.inc
name asm012_140718
_DATA segment word public 'DATA'
; ASMDOC[timers] Block: Private bytes: zero-mode sentinel and saved low BDA equipment byte. Neither is an exported
; ASMDOC[timers]   code entry.
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
; ASMDOC[timers] Contract _video_add_exithandler (proposed name _video_add_exithandler): When saved mode byte is
; ASMDOC[timers]   zero, query BIOS mode, save it and BDA equipment byte, and append video_on_exit to exit list.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS=DGROUP; no stack arguments
; ASMDOC[timers] outputs: No scalar result; nonzero saved mode skips work; mode 0 remains ambiguous sentinel and can
; ASMDOC[timers]   register repeatedly.
; ASMDOC[timers] preserves: No explicit register saves; BIOS and _add_exit_handler effects opaque.
; ASMDOC[timers] clobbers: AX ES flags explicitly; BX via BIOS AH=0Fh; external effects opaque.
; ASMDOC[timers] globals_read: _currentvideomode; 0040:0010 low equipment byte
; ASMDOC[timers] globals_written: _currentvideomode; saved_video_control_register (low BDA equipment byte)
; ASMDOC[timers] services: INT 10h AH=0Fh query current mode; FAR _add_exit_handler(CS:offset _video_on_exit)
; ASMDOC[timers] reachability: Directly called by video_set_mode_13h; installs live exit callback.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_video_add_exithandler proc far
    cmp byte ptr ds:[_currentvideomode], 0
    jne skip_if_saved_mode_exists
; ASMDOC[timers] Block: BIOS query returns mode in AL; saved mode 0 still equals sentinel and therefore does not
; ASMDOC[timers]   prevent later registrations.
    mov ah, 0Fh
    int R5HW_BIOS_VIDEO_INT ;/* PLATFORM(video): calls a BIOS video service. */
    mov byte ptr ds:[_currentvideomode], al
    mov ax, 40h
    mov es, ax
    mov al, byte ptr es:[10h] ;/* PLATFORM(video): accesses display memory through ES. */
    mov byte ptr ds:[saved_video_control_register], al
; ASMDOC[timers] Block: Address-taken FAR cleanup callback makes video_on_exit live without a direct CALL.
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
; ASMDOC[timers] Contract _video_on_exit (proposed name _video_on_exit): Restore saved BDA equipment byte
; ASMDOC[timers]   before/after BIOS mode change; if equipment bits 4-5 are 30h clear graphics memory; set BIOS
; ASMDOC[timers]   background/border zero.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS=DGROUP; no arguments; saved state initialized by video_add_exithandler
; ASMDOC[timers] outputs: No scalar result; saved mode remains stored (not reset).
; ASMDOC[timers] preserves: No explicit saves; BIOS and _video_clear_color preservation applies.
; ASMDOC[timers] clobbers: AX BX ES flags explicitly; external effects opaque (video_clear_color saves DI but writes
; ASMDOC[timers]   CX).
; ASMDOC[timers] globals_read: _currentvideomode; saved_video_control_register
; ASMDOC[timers] globals_written: 0040:0010 low equipment byte; A000 memory conditionally via video_clear_color
; ASMDOC[timers] services: INT 10h AH=00h AL=saved mode; FAR _video_clear_color(word 0) only for saved bits 30h; INT
; ASMDOC[timers]   10h AH=0Bh BX=0
; ASMDOC[timers] reachability: Address taken and registered by video_add_exithandler; live indirect exit entry, not
; ASMDOC[timers]   dead.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
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
; ASMDOC[timers] Block: Test saved equipment display bits, not a hardware control register. Saved state is retained
; ASMDOC[timers]   after restoration.
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
