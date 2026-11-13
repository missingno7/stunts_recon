; ASMDOC[timers] Module: Save entry display state, select BIOS mode 13h, set background/border zero and clear
; ASMDOC[timers]   graphics aperture through existing word-pattern helper.
; ASMDOC[timers] Authoritative contracts below refine older retained comments; original source bytes are untouched.
; ASMDOC[timers] Stack offsets BP+6/BP+8 refer to saved-BP FAR frame; entry SP+4/SP+6 refer to original FAR frame.
.8086
INCLUDE include\hw.inc
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _clearscreen:far
extrn _video_add_exithandler:far
public _initgraphics
; -----------------------------------------------------------------------------
; Purpose: Enter MCGA mode 13h and install the associated cleanup handlers.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, SP, ES; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
;                                                           
;                                                   
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _video_set_mode_13h (proposed name _video_set_mode_13h): Register display cleanup, set
; ASMDOC[timers]   equipment display bits to 10h, select background zero and mode 13h, then clear with word pattern
; ASMDOC[timers]   zero.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS=DGROUP required transitively; no stack arguments
; ASMDOC[timers] outputs: No scalar result; BIOS mode13 display and graphics clear effects.
; ASMDOC[timers] preserves: No explicit saves; BIOS/_video_add_exithandler/_video_clear_color effects apply.
; ASMDOC[timers] clobbers: AX BX ES flags explicitly; CX via clear; external effects opaque.
; ASMDOC[timers] globals_read: 0040:0010 equipment word; _currentvideomode through video_add_exithandler
; ASMDOC[timers] globals_written: 0040:0010 equipment word; A000 memory through video_clear_color; saved
; ASMDOC[timers]   mode/equipment state via video_add_exithandler
; ASMDOC[timers] services: FAR _video_add_exithandler; INT 10h AH=0Bh BX=0; INT 10h AX=0013h; FAR _video_clear_color
; ASMDOC[timers]   with two pushed zero words (only first is read; caller releases 4 bytes)
; ASMDOC[timers] reachability: Startup calls this routine. Existing filename already resolves placeholder owner id;
; ASMDOC[timers]   retain video_set_mode_13h filename.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_initgraphics proc far
; ASMDOC[timers] Block: Capture startup video state before changing equipment flags or invoking BIOS mode setup.
 call far ptr _video_add_exithandler
 mov ax,40h
 mov es,ax
 mov ax,es:[10h] ;                                                          
 and ax,0FFCFh
 or ax,10h
 mov es:[10h],ax ;                                                          
 mov ah,0Bh
 mov bx,0
 int R5HW_BIOS_VIDEO_INT ;                                                  
 mov ah,0
 mov al,13h
 int R5HW_BIOS_VIDEO_INT ;                                                  
; ASMDOC[timers] Block: Two zero words are pushed although video_clear_color reads one word pattern; preserve
; ASMDOC[timers]   original stack sequence and four-byte cleanup.
 xor ax,ax
 push ax
 push ax
 call far ptr _clearscreen
 add sp,4
 ret
_initgraphics endp
S012_TEXT ends
end
