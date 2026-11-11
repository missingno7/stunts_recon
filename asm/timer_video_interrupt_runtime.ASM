; ASMDOC[timers] Module: Fixed-point 2D radius, monochrome/text fallback and IRQ0 timer installation, callback
; ASMDOC[timers]   dispatch and restoration. Mixed code and owned timer/CRTC data; not an audio-driver
; ASMDOC[timers]   implementation.
; ASMDOC[timers] Authoritative contracts below refine older retained comments; original source bytes are untouched.
; ASMDOC[timers] Stack offsets BP+6/BP+8 refer to saved-BP FAR frame; entry SP+4/SP+6 refer to original FAR frame.
; ASMDOC[timers] Data exports: _byte_3F868: 12-byte CRTC setup table (DS:40F8h); _previous_timer_irq_vector: saved
; ASMDOC[timers]   IRQ0 far vector; _timer_callback_counter: dword raw-dispatch counter;
; ASMDOC[timers]   _timer_elapsed_ticks_low/_timer_elapsed_ticks_high: divided elapsed dword;
; ASMDOC[timers]   _timer_sound_countdown_active/_timer_sound_countdown_enabled: byte flags;
; ASMDOC[timers]   _timer_sound_countdown_ticks: word; _timer_interrupt_reload_value/_bios_tick_countdown: word
; ASMDOC[timers]   software divider and current countdown;
; ASMDOC[timers]   _timer_callback_reentry_peak/_timer_callback_reentry_depth: word counters; _timer_callback_busy:
; ASMDOC[timers]   byte guard; _input_pushed: word; ISR tests low byte only; _timerintr: far-pointer list plus zero
; ASMDOC[timers]   sentinel/storage; _aNoRoomLeftOnTimerInterru: NUL-terminated error text
.8086
INCLUDE include\platform_hw.inc
name asm012_131254
DGROUP group DSEG, _DATA
DSEG segment byte public 'STUNTSD'
DSEG ends
assume ds:DGROUP
_DATA segment word public 'DATA'
public _byte_3F868, _previous_timer_irq_vector, _timer_callback_counter
public _timer_elapsed_ticks_low
public _timer_elapsed_ticks_high
public _timer_sound_countdown_active
public _timer_sound_countdown_enabled
public _timer_sound_countdown_ticks
public _timer_interrupt_reload_value
public _bios_tick_countdown
public _timer_callback_reentry_peak
public _timer_callback_reentry_depth
public _timer_callback_busy, _input_pushed, _timerintr, _aNoRoomLeftOnTimerInterru
; ASMDOC[timers] Block: Owned data exports below are storage, not routine entries. The first 12 bytes are the CRTC
; ASMDOC[timers]   table read later via literal DS:40F8h.
; Literal translation or hardware policy table; no analytic generator is inferred.
_byte_3F868 db 61h, 50h, 52h, 0Fh, 19h, 06h, 19h, 19h, 02h, 0Dh, 0Bh, 0Ch
_previous_timer_irq_vector dd 0
_timer_callback_counter dd 0
_timer_elapsed_ticks_low dw 0
_timer_elapsed_ticks_high dw 0
_timer_sound_countdown_active db 00h
_timer_sound_countdown_enabled db 00h
_timer_sound_countdown_ticks dw 0
_timer_interrupt_reload_value dw 0
_bios_tick_countdown dw 0
_timer_callback_reentry_peak dw 0
_timer_callback_reentry_depth dw 0
_timer_callback_busy db 00h
db 03h
_input_pushed dw 0
_timerintr dd 0
db 24 dup (0)
_aNoRoomLeftOnTimerInterru db "NO ROOM LEFT ON TIMER INTERRUPT ROUTINE LIST", 13, 0
_DATA ends
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _polradius2d
public _video_set_mode7
public _nopsub_30180
public _timer_setup_interrupt
public _audio_stop_unknown
public _timer_reg_callback
public _timer_remove_callback
public _compare_ds_ss
public _timer_intr_callback
public _sub_303BA
extrn _cosfast:far
extrn _polang:far
extrn _sinfast:far
extrn _video_mode4_active:byte
extrn _set_bios_mode3:far
extrn _add_exit_handler:far
extrn _fatal_error:far

; polarRadius2D: _polradius2d
; -----------------------------------------------------------------------------
; Purpose: Return the 2D polar radius for the supplied point.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_y (word ptr 8); arg_z
; (word ptr 6).
; Register ABI: touches AX, BX, DX, BP, SP; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _polradius2d (proposed name _polradius2d): Return radius from two signed word components
; ASMDOC[timers]   using reflected polar angle and table sine/cosine, with the original SAR/RCR and unsigned DIV
; ASMDOC[timers]   behavior.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: SS:[BP+6] word z component (entry SP+4); SS:[BP+8] word y component (entry SP+6); DS=DGROUP
; ASMDOC[timers]   for called angle/trig helpers; Incoming AX is retained by _polang when both components are zero;
; ASMDOC[timers]   resulting angle selection/divide fault behavior can depend on it.
; ASMDOC[timers] outputs: AX=unsigned quotient of the quartered DX:AX component dividend by selected table scale;
; ASMDOC[timers]   DX=unsigned remainder; DIV faults remain possible, including -32768 edge cases.
; ASMDOC[timers] preserves: BP and DI restored by this body/callees; SI DS ES unmodified by body and inspected
; ASMDOC[timers]   angle/trig helpers.
; ASMDOC[timers] clobbers: AX BX CX DX arithmetic flags (CX through _polang).
; ASMDOC[timers] globals_read: _sinetable and _atantable through called helpers
; ASMDOC[timers] globals_written:
; ASMDOC[timers] services: FAR _polang(z,y); FAR _cosfast(angle) or _sinfast(angle)
; ASMDOC[timers] reachability: Direct C callers in obj_seg003/005/006 and obj_seg007; live geometry helper.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_polradius2d proc far
    arg_z = word ptr 6
    arg_y = word ptr 8
    push bp
    mov bp, sp
    push word ptr [bp+arg_y]
    push word ptr [bp+arg_z]
    call far ptr _polang
    add sp, 4
    or ax, ax
    jge short radius_reflect_angle_into_range
    neg ax
; ASMDOC[timers] Block: Reflect signed angle into the selected trig quadrant; all NEG/SUB operations retain 16-bit
; ASMDOC[timers]   wrap.
radius_reflect_angle_into_range:
    cmp ax, 100h
    jl short radius_choose_cosine_branch
    sub ax, 200h
    neg ax
radius_choose_cosine_branch:
    cmp ax, 80h
    jg short radius_choose_sine_branch
    push ax
    call far ptr _cosfast
    add sp, 2
    mov bx, ax
    mov dx, [bp+arg_y]
    or dx, dx
    jge short radius_scale_absolute_z
    neg dx
; ASMDOC[timers] Block: Despite legacy label, this branch scales argument y. SAR DX/RCR AX twice forms a quartered
; ASMDOC[timers]   32-bit dividend; DIV BX is unsigned.
radius_scale_absolute_z:
    xor ax, ax
    sar dx, 1
    rcr ax, 1
    sar dx, 1
    rcr ax, 1
    div bx
    pop bp
    retf
radius_choose_sine_branch:
    push ax
    call far ptr _sinfast
    add sp, 2
    mov bx, ax
    mov dx, [bp+arg_z]
    or dx, dx
    jge short radius_scale_absolute_x
    neg dx
; ASMDOC[timers] Block: This branch scales argument z. NEG 8000h stays 8000h; arithmetic shifts and unsigned division
; ASMDOC[timers]   retain the original fault edge.
radius_scale_absolute_x:
    xor ax, ax
    sar dx, 1
    rcr ax, 1
    sar dx, 1
    rcr ax, 1
    div bx
    pop bp
    retf
_polradius2d endp

; video_set_mode7: _video_set_mode7
; -----------------------------------------------------------------------------
; Purpose: Configure the BIOS mode-7 text display.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, CX, DX, DI, DS, ES; balanced explicit saves/restores observed: DI; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _video_mode4_active.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; /* PLATFORM(video): accesses a VGA/MCGA display register port. */
; /* PLATFORM(video): selects a video-memory segment. */
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; /* PLATFORM(video): calls a BIOS video service. */
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _video_set_mode7 (proposed name _video_set_mode7): If video_mode4_active is zero, delegate
; ASMDOC[timers]   to set_bios_mode3. Otherwise set equipment bits, program mono CRTC, clear B800:0000..7FFF and
; ASMDOC[timers]   invoke BIOS mode 7.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS=DGROUP; no stack arguments
; ASMDOC[timers] outputs: Display/BDA changes; no defined scalar return. DF=0 on hardware branch before BIOS; BIOS
; ASMDOC[timers]   may alter flags.
; ASMDOC[timers] preserves: DI explicitly restored; other untouched local registers subject to _set_bios_mode3/BIOS
; ASMDOC[timers]   effects.
; ASMDOC[timers] clobbers: AX BX CX DX ES flags plus opaque BIOS/helper effects.
; ASMDOC[timers] globals_read: _video_mode4_active; _byte_3F868 via literal DS:[BX+40F8h]; 0040:0010 word equipment
; ASMDOC[timers]   flags
; ASMDOC[timers] globals_written: 0040:0010 equipment word; B800:[0000..7FFF] zero-filled (16384 words)
; ASMDOC[timers] services: FAR _set_bios_mode3 on flag=0 path; OUT 3BFh,3B8h,3B4h/3B5h; INT 10h AX=0007h
; ASMDOC[timers] reachability: Shutdown path in src/obj_seg019.c; mode3 alternative is live.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_video_set_mode7 proc far
    push di
    cmp byte ptr ds:[_video_mode4_active], 0
    jne mode7_program_crtc
    call far ptr _set_bios_mode3
    pop di
    retf
; ASMDOC[timers] Block: Hardware fallback uses mono ports but clears B800h (color text aperture), not B000h. Preserve
; ASMDOC[timers]   the observed mismatch.
mode7_program_crtc:
    mov ax, 40h
    mov es, ax
    mov ax, es:[10h] ;/* PLATFORM(video): accesses display memory through ES. */
    and ax, 0FFCFh
    or ax, 30h
    mov es:[10h], ax ;/* PLATFORM(video): accesses display memory through ES. */
    mov al, 3
    mov dx, R5HW_VIDEO_MONO_MISC_PORT
    out dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    mov al, 20h
    mov dx, R5HW_VIDEO_MONO_MODE_PORT
    out dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    mov cx, 0Ch
    mov dx, R5HW_VIDEO_MONO_CRTC_INDEX_PORT
    xor bx, bx
; ASMDOC[timers] Block: Write 12 CRTC index/value pairs from owned table at DGROUP:40F8h.
mode7_write_crtc_register_loop:
    mov al, bl
    out dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    inc dx
    mov al, [bx + 40F8h]
    out dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    dec dx
    inc bx
    loop mode7_write_crtc_register_loop
    cld
    mov ax, R5HW_VIDEO_TEXT_SEGMENT ;/* PLATFORM(video): selects a video-memory segment. */
    mov es, ax
    xor di, di
    mov cx, 4000h
    xor ax, ax
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    mov al, 28h
    mov dx, R5HW_VIDEO_MONO_MODE_PORT
    out dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    mov ah, 0
    mov al, 7
    int R5HW_BIOS_VIDEO_INT ;/* PLATFORM(video): calls a BIOS video service. */
    pop di
    retf
_video_set_mode7 endp

; nopsub_30180: _nopsub_30180
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 30180.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; no named stack arguments referenced.
; Register ABI: touches DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _bios_tick_countdown, _timer_interrupt_reload_value, _timer_sound_countdown_active,
; _timer_sound_countdown_ticks.
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _nopsub_30180 (proposed name _timer_setup_sound_countdown): Alternate setup: DX=2E9Ch,
; ASMDOC[timers]   software divider/countdown=5, sound countdown=100 and active=1; jump into shared timer
; ASMDOC[timers]   installation.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS=DGROUP; no stack arguments; existing _timer_sound_countdown_enabled is retained
; ASMDOC[timers] outputs: IRQ0 installation as timer_setup_interrupt; no scalar return; IF forced enabled on normal
; ASMDOC[timers]   path.
; ASMDOC[timers] preserves: BP pushed here and popped by shared_setup; no other local save. Opaque exit-list call
; ASMDOC[timers]   effects.
; ASMDOC[timers] clobbers: AX BX DX ES flags; opaque _add_exit_handler effects.
; ASMDOC[timers] globals_read: 0000:0020/0022 IRQ0 vector
; ASMDOC[timers] globals_written: _timer_interrupt_reload_value; _bios_tick_countdown; _timer_sound_countdown_active;
; ASMDOC[timers]   _timer_callback_busy; _timerintr[0] (first far-pointer sentinel only); _previous_timer_irq_vector
; ASMDOC[timers]   (conditionally); 0000:0020/0022 IRQ0 vector (conditionally); _timer_sound_countdown_ticks
; ASMDOC[timers] services: CLI/STI (forces IF=1 before return); IN/OUT 61h speaker control; OUT 43h=B6h (channel2
; ASMDOC[timers]   command), OUT 40h=DL then DH (channel0 data); IN/OUT 21h masks then unmasks IRQ0/1; FAR
; ASMDOC[timers]   _add_exit_handler(CS:1848h = _audio_stop_unknown)
; ASMDOC[timers] reachability: No references to public or address found in active source/recipes beyond declaration;
; ASMDOC[timers]   no callback/vector installation targets this alternate entry. Shared tail remains live from normal
; ASMDOC[timers]   setup.
; ASMDOC[timers] dead: true (static known-program reachability; never permission to trim extent)
_nopsub_30180 proc far
    push    bp
    mov     bp, sp
    mov     dx, 2E9Ch
    mov     _timer_interrupt_reload_value, 5
    mov     _bios_tick_countdown, 5
    mov     _timer_sound_countdown_ticks, 64h ; 'd'
    mov     _timer_sound_countdown_active, 1
    jmp     short shared_setup
    even ; word-align the following entry

_nopsub_30180 endp

; nopsub_30180: _timer_setup_interrupt
; -----------------------------------------------------------------------------
; Purpose: Install and configure the game timer interrupt handler.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, DX, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: none
; inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _bios_tick_countdown, _previous_timer_irq_vector, _timer_callback_busy,
; _timer_interrupt_reload_value, _timer_sound_countdown_active,
;   _timer_sound_countdown_enabled, _timer_sound_countdown_ticks, _timerintr.
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
; /* PLATFORM(timer): accesses the programmable interval timer port. */
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _timer_setup_interrupt (proposed name _timer_setup_interrupt): Set default DX=2E9Ch and
; ASMDOC[timers]   software divider=5; disable active countdown, enable chaining, clear first callback slot and
; ASMDOC[timers]   install IRQ0 plus cleanup handler.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS=DGROUP; public entry has no stack arguments. Two unexported skipped interior setup
; ASMDOC[timers]   variants instead read SS:[BP+6] word divisor.
; ASMDOC[timers] outputs: IRQ0 and speaker/PIT/PIC state configured; no scalar return; IF forced enabled. Existing
; ASMDOC[timers]   callback_counter, elapsed counter and countdown_ticks are not reset.
; ASMDOC[timers] preserves: BP explicitly restored; no other local saves; _add_exit_handler effects opaque.
; ASMDOC[timers] clobbers: AX BX DX ES flags plus external call effects.
; ASMDOC[timers] globals_read: 0000:0020/0022 IRQ0 vector
; ASMDOC[timers] globals_written: _timer_interrupt_reload_value; _bios_tick_countdown; _timer_sound_countdown_active;
; ASMDOC[timers]   _timer_callback_busy; _timerintr[0] (first far-pointer sentinel only); _previous_timer_irq_vector
; ASMDOC[timers]   (conditionally); 0000:0020/0022 IRQ0 vector (conditionally); _timer_sound_countdown_enabled
; ASMDOC[timers] services: CLI/STI (forces IF=1 before return); IN/OUT 61h speaker control; OUT 43h=B6h (channel2
; ASMDOC[timers]   command), OUT 40h=DL then DH (channel0 data); IN/OUT 21h masks then unmasks IRQ0/1; FAR
; ASMDOC[timers]   _add_exit_handler(CS:1848h = _audio_stop_unknown)
; ASMDOC[timers] reachability: Startup calls public entry. Literal CS:1909h installed in IVT is _timer_intr_callback;
; ASMDOC[timers]   literal CS:1848h registered for cleanup. Interior variants have no label/public/reference and are
; ASMDOC[timers]   bypassed by unconditional jump.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_timer_setup_interrupt proc far
 push bp
 mov dx,2E9Ch
 mov word ptr ds:[_timer_interrupt_reload_value],5
 mov word ptr ds:[_bios_tick_countdown],5
 mov byte ptr ds:[_timer_sound_countdown_active],0
 mov byte ptr ds:[_timer_sound_countdown_enabled],1
 jmp shared_setup
; ASMDOC[timers] Block: Two skipped, unexported setup variants follow the default-entry jump. Each reads BP+6 as a
; ASMDOC[timers]   divisor; no incoming branch is present.
 push bp
 mov bp,sp
 mov dx,1
 xor ax,ax
 idiv word ptr [bp+6]
 mov ds:[_timer_interrupt_reload_value],ax
 mov ds:[_bios_tick_countdown],ax
 mov dx,[bp+6]
 mov byte ptr ds:[_timer_sound_countdown_active],0
 mov byte ptr ds:[_timer_sound_countdown_enabled],1
 jmp shared_setup
 push bp
 mov bp,sp
 mov dx,1
 xor ax,ax
 idiv word ptr [bp+6]
 mov ds:[_timer_interrupt_reload_value],ax
 mov ds:[_bios_tick_countdown],ax
 mov dx,[bp+6]
 mov word ptr ds:[_timer_sound_countdown_ticks],64h
 mov byte ptr ds:[_timer_sound_countdown_active],1
; ASMDOC[timers] Block: Shared tail entered with BP saved and PIT data in DX. Clear only first callback slot; do not
; ASMDOC[timers]   reset the counters or whole list.
shared_setup:
 cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
 mov byte ptr ds:[_timer_callback_busy],0
 mov word ptr ds:[_timerintr],0
 mov word ptr ds:[_timerintr+2],0
 sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
 in al,R5HW_KEYBOARD_CONTROL_PORT ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
 and al,0FCh
 out R5HW_KEYBOARD_CONTROL_PORT,al ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
; ASMDOC[timers] Block: B6h commands PIT channel2, whereas DL/DH below go to channel0 port40h. This is observed code,
; ASMDOC[timers]   not a measured-rate claim.
 mov al,0B6h
 out R5HW_PIT_CONTROL_PORT,al ;/* PLATFORM(timer): accesses the programmable interval timer port. */
 in al,R5HW_PIC_MASTER_MASK_PORT ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
 or al,03h
 out R5HW_PIC_MASTER_MASK_PORT,al ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
 xor ax,ax
 mov es,ax
 cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
 mov ax,es:[20h]
 cmp ax,1909h
 je setup_after_old_off
 mov  word ptr ds:[_previous_timer_irq_vector],ax
; ASMDOC[timers] Block: Offset and segment ownership checks are separate. If old segment already equals CS, vector
; ASMDOC[timers]   installation is skipped even when its offset differs.
setup_after_old_off:
 mov ax,es:[22h]
 mov bx,cs
 cmp ax,bx
 je setup_after_old_seg
 mov  word ptr ds:[_previous_timer_irq_vector+2],ax
 mov word ptr es:[20h],1909h
 mov word ptr es:[22h],cs
setup_after_old_seg:
 in al,R5HW_PIC_MASTER_MASK_PORT ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
 and al,0FCh
 out R5HW_PIC_MASTER_MASK_PORT,al ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
 sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
 mov al,dl
 out R5HW_PIT_CHANNEL0_PORT,al ;/* PLATFORM(timer): accesses the programmable interval timer port. */
 mov al,dh
 out R5HW_PIT_CHANNEL0_PORT,al ;/* PLATFORM(timer): accesses the programmable interval timer port. */
 push cs
 mov ax,1848h
 push ax
 call far ptr _add_exit_handler
 add sp,4
 pop bp
 ret

_timer_setup_interrupt endp

; audio_stop_unk: _audio_stop_unknown
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by audio stop unknown.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, DX, CS, ES; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _previous_timer_irq_vector.
; Globals written: none identified.
; /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; /* PLATFORM(timer): accesses the programmable interval timer port. */
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _audio_stop_unknown (proposed name _timer_restore_irq_and_silence): If IRQ0 is still
; ASMDOC[timers]   CS:1909h, restore prior vector, unmask IRQ0/1, write two zero bytes to PIT channel0 and disable
; ASMDOC[timers]   speaker gate bits 0-1. Otherwise return immediately.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS=DGROUP; CS identifies installed owner; no stack arguments
; ASMDOC[timers] outputs: No scalar return; on owner path IF=1, ES=0; on early return interrupt state unchanged.
; ASMDOC[timers] preserves: BX CX SI DI BP DS unmodified locally.
; ASMDOC[timers] clobbers: AX DX ES flags
; ASMDOC[timers] globals_read: _previous_timer_irq_vector; 0000:0020/0022 IRQ0 vector
; ASMDOC[timers] globals_written: 0000:0020/0022 IRQ0 vector
; ASMDOC[timers] services: IN/OUT 21h; CLI/STI; OUT 40h twice with zero; IN/OUT 61h clears bits 0-1
; ASMDOC[timers] reachability: Direct shutdown callers plus exit list registered through CS:1848h, not dead.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_audio_stop_unknown proc far

    xor     ax, ax
    mov     es, ax
    mov     ax, es:22h
    mov     dx, cs
    cmp     ax, dx
    jz      short timer_restore_owner_check
    retf
timer_restore_owner_check:
    mov     ax, es:20h
    cmp     ax, 1909h
    jz      short timer_restore_prev_irq_vector
    retf
timer_restore_prev_irq_vector:
    in      al, R5HW_PIC_MASTER_MASK_PORT
; Interrupt controller, 8259A. /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
    or      al, 03h
    out     R5HW_PIC_MASTER_MASK_PORT, al
; Interrupt controller, 8259A. /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov     ax, word ptr _previous_timer_irq_vector
    mov     es:20h, ax
    mov     ax, word ptr _previous_timer_irq_vector+2
    mov     es:22h, ax
    in      al, R5HW_PIC_MASTER_MASK_PORT
; Interrupt controller, 8259A. /* PLATFORM(dos): accesses the PIC or keyboard controller port. */

    and     al, 0FCh

    out     R5HW_PIC_MASTER_MASK_PORT, al
; Interrupt controller, 8259A. /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov     al, 0
    out     R5HW_PIT_CHANNEL0_PORT, al
; Timer 8253-5 (AT: 8254.2). /* PLATFORM(timer): accesses the programmable interval timer port. */
    out     R5HW_PIT_CHANNEL0_PORT, al
; Timer 8253-5 (AT: 8254.2). /* PLATFORM(timer): accesses the programmable interval timer port. */
    in      al, R5HW_KEYBOARD_CONTROL_PORT
; PC/XT PPI port B bits: /* PLATFORM(dos): accesses the PIC or keyboard controller port. */

    and     al, 0FCh

    out     R5HW_KEYBOARD_CONTROL_PORT, al
; PC/XT PPI port B bits: /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
    retf
_audio_stop_unknown endp

; timer_reg_callback: _timer_reg_callback
; -----------------------------------------------------------------------------
; Purpose: Register a timer callback in the callback table.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BX, CX, BP, SP; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _aNoRoomLeftOnTimerInterru, _timerintr.
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _timer_reg_callback (proposed name _timer_reg_callback): Append a far callback at the first
; ASMDOC[timers]   slot with zero segment among five slots; publish segment last and zero following sentinel segment.
; ASMDOC[timers]   Full list calls fatal_error.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: SS:[BP+6] word callback offset (entry SP+4); SS:[BP+8] word callback segment (entry SP+6;
; ASMDOC[timers]   zero marks empty); DS=DGROUP
; ASMDOC[timers] outputs: No scalar return; AX=callback segment on successful path.
; ASMDOC[timers] preserves: BP restored; DX SI DI DS ES unmodified locally on success.
; ASMDOC[timers] clobbers: AX BX CX flags; failure-path _fatal_error effects opaque.
; ASMDOC[timers] globals_read: _timerintr segments at offsets 2,6,10,14,18; _aNoRoomLeftOnTimerInterru on full-list
; ASMDOC[timers]   path
; ASMDOC[timers] globals_written: _timerintr selected slot offset+segment; _timerintr following slot segment sentinel
; ASMDOC[timers] services: FAR _fatal_error(DS-relative NUL string); assumed nonreturning: otherwise code falls into
; ASMDOC[timers]   append with BX beyond five entries and unbalanced pushed argument
; ASMDOC[timers] reachability: set_frame_callback and audio driver registration callers. FAR pointer, not a
; ASMDOC[timers]   callback/context pair; five slots plus sentinel.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_timer_reg_callback proc far
    arg_callback_offset = word ptr 6
    arg_callback_segment = word ptr 8

    push    bp
    mov     bp, sp
    mov     cx, 5
    mov     bx, offset DGROUP:_timerintr
; ASMDOC[timers] Block: Five far callback slots are identified by nonzero segment, with a following zero-segment
; ASMDOC[timers]   sentinel; no callback context word exists.
timer_callback_find_free_slot:
    cmp     word ptr [bx+2], 0
    jz      short timer_callback_slot_available
    add     bx, 4
    loop    timer_callback_find_free_slot
    mov     ax, offset DGROUP:_aNoRoomLeftOnTimerInterru; "NO ROOM LEFT ON TIMER INTERRUPT ROUTINE"...
    push    ax
    call    far ptr _fatal_error
timer_callback_slot_available:
    mov     ax, [bp+arg_callback_offset]
    mov     [bx], ax
    mov     word ptr [bx+2], 0
    mov     ax, [bp+arg_callback_segment]
    mov     [bx+2], ax
    mov     word ptr [bx+6], 0
    pop     bp
    retf
_timer_reg_callback endp

; timer_remove_callback: _timer_remove_callback
; -----------------------------------------------------------------------------
; Purpose: Remove a timer callback from the callback table.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, BP, SP; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _timerintr.
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _timer_remove_callback (proposed name _timer_remove_callback): Match both words of a far
; ASMDOC[timers]   callback in five slots; compact remaining slots and clear last slot under CLI/STI.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: SS:[BP+6] word callback offset (entry SP+4); SS:[BP+8] word callback segment (entry SP+6);
; ASMDOC[timers]   DS=DGROUP
; ASMDOC[timers] outputs: No scalar result. On match IF=1; on no match incoming IF unchanged.
; ASMDOC[timers] preserves: BP restored; SI DI DS ES unmodified.
; ASMDOC[timers] clobbers: AX BX CX DX flags
; ASMDOC[timers] globals_read: _timerintr[0..19]
; ASMDOC[timers] globals_written: _timerintr matched suffix through slot4 (only on match)
; ASMDOC[timers] services: CLI/STI around compaction
; ASMDOC[timers] reachability: remove_frame_callback and audio timer teardown callers.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_timer_remove_callback proc far
    arg_callback_offset = word ptr 6
    arg_callback_segment = word ptr 8

    push    bp
    mov     bp, sp
    mov     cx, 5
    mov     bx, offset DGROUP:_timerintr
    mov     ax, [bp+arg_callback_offset]
    mov     dx, [bp+arg_callback_segment]
timer_remove_search_slot_loop:
    cmp     [bx], ax
    jnz     short timer_remove_advance_slot
    cmp     [bx+2], dx
    jz      short timer_remove_disable_interrupts
timer_remove_advance_slot:
    add     bx, 4
    loop    timer_remove_search_slot_loop
    pop     bp
    retf
; ASMDOC[timers] Block: Only successful compaction masks interrupts; STI enables them unconditionally rather than
; ASMDOC[timers]   restoring entry IF.
timer_remove_disable_interrupts:
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    dec     cx
    jz      short timer_remove_clear_tail_slot
timer_remove_shift_slots:
    mov     ax, [bx+4]
    mov     [bx], ax
    mov     ax, [bx+6]
    mov     [bx+2], ax
    add     bx, 4
    loop    timer_remove_shift_slots
timer_remove_clear_tail_slot:
    mov     word ptr [bx], 0
    mov     word ptr [bx+2], 0
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    pop     bp
    retf
_timer_remove_callback endp

; compare_ds_ss: _compare_ds_ss
; -----------------------------------------------------------------------------
; Purpose: Implement the timer service identified by compare ds ss.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, DX, DS, SS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _compare_ds_ss (proposed name _compare_ds_ss): Return whether the current data and stack
; ASMDOC[timers]   segment selectors are equal.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS and SS register values; no stack arguments
; ASMDOC[timers] outputs: AX=1 if DS=SS, otherwise AX=0; BX=SS; DX=DS; ZF reflects comparison.
; ASMDOC[timers] preserves: CX SI DI BP DS ES SS unmodified.
; ASMDOC[timers] clobbers: AX BX DX arithmetic flags
; ASMDOC[timers] globals_read:
; ASMDOC[timers] globals_written:
; ASMDOC[timers] services:
; ASMDOC[timers] reachability: Replay and audio paths directly test this result.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_compare_ds_ss proc far

    xor     ax, ax
    mov     bx, ss
    mov     dx, ds
    cmp     bx, dx
    jnz     short compare_equal_segment_return
    inc     ax
compare_equal_segment_return:
    retf
_compare_ds_ss endp

; timer_intr_group: _timer_intr_callback
; -----------------------------------------------------------------------------
; Purpose: Dispatch the timer interrupt callback and acknowledge the interrupt.
; ABI: FAR entry; source return form: IRET; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, DS, ES; balanced explicit saves/restores observed: AX, BX,
; CX, DI, DS, DX, ES, SI; path-specific inputs and outputs follow
;   the listed operands.
; Globals read: _bios_tick_countdown, _timer_callback_busy, _timer_callback_counter,
; _timer_callback_reentry_depth, _timer_callback_reentry_peak, _timer_elapsed_ticks_high,
; _timer_elapsed_ticks_low, _timer_interrupt_reload_value, _timer_sound_countdown_enabled, _timerintr,
; _input_pushed.
; Globals written: _bios_tick_countdown, _timer_callback_busy, _timer_callback_counter,
; _timer_callback_reentry_depth, _timer_callback_reentry_peak, _timer_elapsed_ticks_high,
;   _timer_elapsed_ticks_low.
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; /* PLATFORM(dos): accesses the PIC or keyboard controller port. */
; /* PLATFORM(timer): returns from a hardware timer interrupt. */
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _timer_intr_callback (proposed name _timer_intr_callback): IRQ0 handler: divided
; ASMDOC[timers]   elapsed/BIOS chain work, raw callback counter and live far-pointer callback scan guarded against
; ASMDOC[timers]   reentry.
; ASMDOC[timers] convention: Interrupt FAR entry; interrupt frame, explicit save set, IRET; never ordinary cdecl
; ASMDOC[timers]   return.
; ASMDOC[timers] inputs: Hardware interrupt frame at entry SS:[SP+0]=IP, [SP+2]=CS, [SP+4]=FLAGS; No arguments;
; ASMDOC[timers]   incoming DS arbitrary; handler loads DGROUP; _input_pushed low byte gates callback
; ASMDOC[timers]   counting/scanning only
; ASMDOC[timers] outputs: IRET restores interrupted IP/CS/FLAGS. Elapsed ticks increment every software-divider
; ASMDOC[timers]   expiry; callback counter increments every IRQ with input_pushed low byte=0, including
; ASMDOC[timers]   busy/reentrant IRQs.
; ASMDOC[timers] preserves: AX BX CX DX SI DI BP DS ES explicitly saved/restored; SS/SP must remain balanced across
; ASMDOC[timers]   opaque callbacks; IRET restores original FLAGS.
; ASMDOC[timers] clobbers: Transient AX DI DS and flags locally; callbacks and prior IRQ handler may change arbitrary
; ASMDOC[timers]   registers/memory. Their preservation of DS/DI is required by subsequent scan; DF is not explicitly
; ASMDOC[timers]   cleared.
; ASMDOC[timers] globals_read: _bios_tick_countdown; _timer_interrupt_reload_value; _timer_sound_countdown_enabled;
; ASMDOC[timers]   _input_pushed low byte; _timer_callback_busy; _timerintr live list; _timer_callback_reentry_depth;
; ASMDOC[timers]   _timer_callback_reentry_peak; _timer_elapsed_ticks_low/high; _timer_callback_counter
; ASMDOC[timers] globals_written: _bios_tick_countdown; _timer_elapsed_ticks_low/high; _timer_callback_counter;
; ASMDOC[timers]   _timer_callback_busy; _timer_callback_reentry_depth; _timer_callback_reentry_peak; opaque
; ASMDOC[timers]   callback/prior IRQ effects
; ASMDOC[timers] services: CLI/STI with nested IRQs permitted; OUT 20h=20h except when chaining old IRQ; NEAR
; ASMDOC[timers]   timer_sound_countdown_entry on divided tick when enabled; CALL DWORD PTR _timerintr[DI] (FAR
; ASMDOC[timers]   callbacks, no arguments); IRET
; ASMDOC[timers] reachability: Live hardware entry, installed as literal CS:1909h at IVT 0000:0020h. Not dead despite
; ASMDOC[timers]   no direct CALL.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_timer_intr_callback proc far
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    push ds
    push es
    push ax
    push bx
    push cx
    push dx
    push si
    push di
    push bp
    mov ax, seg dseg
    mov ds, ax
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    dec word ptr _bios_tick_countdown
    jg timer_send_pic_eoi
    inc word ptr _timer_elapsed_ticks_low
    jne timer_reload_tick_counter
    inc word ptr _timer_elapsed_ticks_high
; ASMDOC[timers] Block: Divided elapsed counter and BIOS chaining run only on countdown expiry. Callback counting
; ASMDOC[timers]   below runs on every IRQ.
timer_reload_tick_counter:
    mov ax, word ptr _timer_interrupt_reload_value
    mov word ptr _bios_tick_countdown, ax
    cmp byte ptr _timer_sound_countdown_enabled, 0
    je timer_send_pic_eoi
    call timer_sound_countdown_entry
    jmp short timer_check_callback_enable
    nop
timer_send_pic_eoi:
    mov al, 20h
    out R5HW_PIC_MASTER_COMMAND_PORT, al ;/* PLATFORM(dos): accesses the PIC or keyboard controller port. */
; ASMDOC[timers] Block: Test only low byte of input_pushed. A nonzero value skips callback counter and clears
; ASMDOC[timers]   busy/reentry state, even on nested entry.
timer_check_callback_enable:
    cmp byte ptr _input_pushed, 0
    jne timer_clear_reentry_state
    inc word ptr _timer_callback_counter
    jne timer_advance_callback_clock
    inc word ptr _timer_callback_counter+2
timer_advance_callback_clock:
    xor di, di
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    cmp byte ptr _timer_callback_busy, 0
    jne timer_count_reentrant_tick
    mov byte ptr _timer_callback_busy, 1
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
; ASMDOC[timers] Block: Read each live FAR slot after the previous callback returns; callbacks must preserve DS and
; ASMDOC[timers]   DI for this loop. Effects on other globals are opaque.
timer_start_callback_scan:
    mov ax, word ptr _timerintr[di+2]
    or ax, ax
    je timer_clear_reentry_state
    call dword ptr _timerintr[di]
    add di, 4
    jmp timer_start_callback_scan
; ASMDOC[timers] Block: Busy nested IRQs still advanced callback_counter; record signed maximum depth, then return
; ASMDOC[timers]   without dispatching callbacks.
timer_count_reentrant_tick:
    inc word ptr _timer_callback_reentry_depth
    mov ax, word ptr _timer_callback_reentry_depth
    cmp word ptr _timer_callback_reentry_peak, ax
    jge timer_restore_irq_frame
    mov word ptr _timer_callback_reentry_peak, ax
    jmp short timer_restore_irq_frame
    nop
timer_clear_reentry_state:
    mov byte ptr _timer_callback_busy, 0
    mov word ptr _timer_callback_reentry_depth, 0
timer_restore_irq_frame:
    pop bp
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    pop es
    pop ds
    iret ;/* PLATFORM(timer): returns from a hardware timer interrupt. */
_timer_intr_callback endp

; timer_intr_group: _sub_303BA
; -----------------------------------------------------------------------------
; Purpose: Advance the timer sound countdown and chain to the previous timer interrupt.
; ABI: NEAR entry; source return form: RET mnemonic for NEAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: _timer_sound_countdown_active, _timer_sound_countdown_ticks.
; Globals written: _timer_sound_countdown_active, _timer_sound_countdown_enabled, _timer_sound_countdown_ticks.
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _sub_303BA (proposed name _timer_tick_sound_and_chain): On divided timer tick decrement
; ASMDOC[timers]   active signed sound countdown, disable countdown/chaining at expiry, then always invoke saved IRQ0
; ASMDOC[timers]   via PUSHF/FAR CALL.
; ASMDOC[timers] convention: NEAR call/RETN; DS=DGROUP; synthesizes an interrupt frame for saved FAR IRQ handler.
; ASMDOC[timers] inputs: DS=DGROUP; near return IP at SS:[SP]; no arguments; _previous_timer_irq_vector must target
; ASMDOC[timers]   an interrupt-compatible handler
; ASMDOC[timers] outputs: No scalar return; effects of old IRQ handler opaque; near return after its IRET consumes
; ASMDOC[timers]   synthesized frame.
; ASMDOC[timers] preserves: No local GPR/segment saves or writes; all preservation depends on previous IRQ handler.
; ASMDOC[timers] clobbers: Arithmetic flags locally; arbitrary transitive old IRQ effects.
; ASMDOC[timers] globals_read: _timer_sound_countdown_active; _timer_sound_countdown_ticks;
; ASMDOC[timers]   _previous_timer_irq_vector
; ASMDOC[timers] globals_written: _timer_sound_countdown_ticks; _timer_sound_countdown_active;
; ASMDOC[timers]   _timer_sound_countdown_enabled; opaque prior IRQ effects
; ASMDOC[timers] services: PUSHF; FAR CALL _previous_timer_irq_vector (returns via IRET); RETN
; ASMDOC[timers] reachability: Called by IRQ0 via local alias timer_sound_countdown_entry at same address as public.
; ASMDOC[timers]   Live.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_sub_303BA proc near
; ASMDOC[timers] Block: Same address as public timer_tick_sound_and_chain; local alias is a real incoming call edge.
timer_sound_countdown_entry:
    cmp byte ptr _timer_sound_countdown_active, 0
    je timer_chain_previous_timer_irq
    dec word ptr _timer_sound_countdown_ticks
    jg timer_chain_previous_timer_irq
    mov byte ptr _timer_sound_countdown_active, 0
    mov byte ptr _timer_sound_countdown_enabled, 0
; ASMDOC[timers] Block: PUSHF plus FAR CALL supplies an interrupt-compatible frame; saved handler returns by IRET.
; ASMDOC[timers]   Its effects are opaque.
timer_chain_previous_timer_irq:
    pushf
    call dword ptr _previous_timer_irq_vector
    retn
    db 0 ; retained zero fill at this boundary
_sub_303BA endp
S012_TEXT ends
end
