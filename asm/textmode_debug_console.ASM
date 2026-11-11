; ASMDOC[timers] Module: Monochrome 80x25 diagnostic console with word character/attribute cells, unbounded sprintf
; ASMDOC[timers]   staging, cursor controls and two unexported unreachable interior entry fragments.
; ASMDOC[timers] Authoritative contracts below refine older retained comments; original source bytes are untouched.
; ASMDOC[timers] Stack offsets BP+6/BP+8 refer to saved-BP FAR frame; entry SP+4/SP+6 refer to original FAR frame.
.8086
INCLUDE include\platform_hw.inc
DSEG segment byte public 'STUNTSD'
DSEG ends
_DATA segment word public 'DATA'
; ASMDOC[timers] Block: Private console state plus 120-byte output buffer; no data labels are public. Return slots
; ASMDOC[timers]   and sprintf buffer make printing non-reentrant.
console_cursor_row db 00h
console_save_cursor db 00h
console_text_attribute db 07h
db 00h
console_saved_cursor dw 3840
console_cursor_offset dw 0
console_return_ip dw 0
console_return_cs dw 0
_output_buffer db 120 dup (0)
_DATA ends
DGROUP group _DATA,DSEG
assume ds:DGROUP
extrn _sprintf:far
public _console_clear_mono_screen
public _console_delete_current_row
public _debug_printf_text
public _console_set_text_attribute
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT

; console_clear_mono_screen
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 3215A.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, CX, DI, ES; balanced explicit saves/restores observed: DI; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): selects a video-memory segment. */
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _nopsub_3215A (proposed name _console_clear_mono_screen): Clear all 2000 monochrome cells
; ASMDOC[timers]   with zero words, leaving caller cursor state unchanged.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: No arguments; ignores incoming ES and sets B000h
; ASMDOC[timers] outputs: ES=B000h; AX=0; CX=0; DF=0; B000:[0000..0F9F]=0; DI restored.
; ASMDOC[timers] preserves: DI explicitly restored; BX DX SI BP DS unmodified.
; ASMDOC[timers] clobbers: AX CX ES flags (DF cleared)
; ASMDOC[timers] globals_read:
; ASMDOC[timers] globals_written: B000:[0000..0F9F] text page
; ASMDOC[timers] services: REP STOSW 2000 words
; ASMDOC[timers] reachability: Called by debug_printf_text form-feed control; live despite legacy nopsub name.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_console_clear_mono_screen proc far
    push di
    mov ax, R5HW_VIDEO_MONO_TEXT_SEGMENT ;/* PLATFORM(video): selects a video-memory segment. */
    mov es, ax
    xor di, di
    mov cx, 07D0h
    xor ax, ax
    cld
    rep stosw
    pop di
    retf
_console_clear_mono_screen endp

; console_delete_current_row
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 3216C.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, CX, SI, DI, DS, ES; balanced explicit saves/restores observed: DI, DS, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: console_cursor_row.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _nopsub_3216C (proposed name _console_delete_current_row): Delete configured row by moving
; ASMDOC[timers]   following text rows upward, then zero last row; default row=0 scrolls whole page.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS=DGROUP; ES=text page segment supplied by caller (debug_printf_text sets B000h);
; ASMDOC[timers]   console_cursor_row byte normally 0..24; no bounds check here
; ASMDOC[timers] outputs: ES page updated from row*160 to end; CX=0; AX=0; BL=50h; DF=0. Row outside 0..24 produces
; ASMDOC[timers]   wrapped byte-count arithmetic.
; ASMDOC[timers] preserves: SI DI DS explicitly restored; DX BP ES unchanged.
; ASMDOC[timers] clobbers: AX BL CX flags (BH unchanged locally, DF cleared)
; ASMDOC[timers] globals_read: console_cursor_row
; ASMDOC[timers] globals_written: ES:row*160..0F9F for valid row
; ASMDOC[timers] services: REP MOVSW (24-row)*80 words; REP STOSW 80 words
; ASMDOC[timers] reachability: debug_printf_text overflow/newline paths call this far routine.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_console_delete_current_row proc far
    push    si
    push    di
    push    ds
; ASMDOC[timers] Block: For valid row 0..24, copy (24-row)*80 word cells upward then clear final 80 cells; ES is
; ASMDOC[timers]   supplied by caller.
    mov     al, console_cursor_row
    mov     bl, 0A0h
    mul     bl
    mov     si, ax
    mov     di, si
    add     si, 0A0h
    mov     ax, 18h
    sub     al, console_cursor_row
    mov     bl, 50h
    mul     bl
    mov     cx, ax
    mov     ax, es
    mov     ds, ax
    cld
    rep movsw
    xor     ax, ax
    mov     cx, 50h
    rep stosw
    pop     ds
    pop     di
    pop     si
    retf
_console_delete_current_row endp

; nopsub_3219D
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by debug printf text.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, DX, SI, BP, SP, ES; balanced explicit saves/restores observed: AX, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: console_cursor_offset, console_return_cs, console_return_ip, console_save_cursor,
; console_saved_cursor, console_text_attribute.
; Globals written: console_cursor_offset, console_cursor_row, console_return_cs, console_return_ip,
; console_save_cursor, console_saved_cursor.
; Global addresses taken: _output_buffer.
; /* PLATFORM(video): selects a video-memory segment. */
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _debug_printf_text (proposed name _debug_printf_text): Format caller varargs into static
; ASMDOC[timers]   120-byte buffer then render legacy bytes on B000 text page using saved cursor and attribute;
; ASMDOC[timers]   handles LF, BS, FF and TAB.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: At FAR entry SS:[SP+4] near format-string pointer, [SP+6...] varargs; caller cleans original
; ASMDOC[timers]   arguments; DS=DGROUP; sprintf destination supplied as extra near pointer after popping original
; ASMDOC[timers]   return address; console_saved_cursor default 3840; static return slots make this non-reentrant
; ASMDOC[timers] outputs: No scalar result; console_saved_cursor advances after output; SI restored around rendering
; ASMDOC[timers]   only, subject to sprintf ABI; reconstructed FAR return frame. Signed byte test ignores 80h..FFh as
; ASMDOC[timers]   controls.
; ASMDOC[timers] preserves: SI saved/restored after _sprintf; any other preservation depends on called
; ASMDOC[timers]   sprintf/helpers; SS/SP must remain balanced.
; ASMDOC[timers] clobbers: AX BX CX DX ES flags locally/transitively; _sprintf effects opaque; DI locally
; ASMDOC[timers]   helper-preserved, BP not written on reachable path; DF may clear via clear/scroll.
; ASMDOC[timers] globals_read: console_saved_cursor; console_cursor_offset; console_save_cursor;
; ASMDOC[timers]   console_text_attribute; console_return_cs/ip; _output_buffer via literal DS:4BC6h;
; ASMDOC[timers]   console_cursor_row via scroll
; ASMDOC[timers] globals_written: console_return_ip/cs; _output_buffer (120-byte allocation; no sprintf bound);
; ASMDOC[timers]   console_save_cursor; console_cursor_offset; console_saved_cursor; B000 text page
; ASMDOC[timers] services: FAR _sprintf(destination, original format/varargs); FAR _nopsub_3216C scroll; FAR
; ASMDOC[timers]   _nopsub_3215A form feed clear
; ASMDOC[timers] reachability: Direct audio diagnostic callers. Interior positioned-print and row-set fragments after
; ASMDOC[timers]   unconditional JMP are not public and have no incoming branch/reference.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_debug_printf_text proc far
; ASMDOC[timers] Block: Remove original FAR return frame so pushing destination exposes original format and varargs
; ASMDOC[timers]   directly to sprintf; save return words globally.
    pop ax
    mov word ptr console_return_ip, ax
    pop ax
    mov word ptr console_return_cs, ax
    mov ax, offset DGROUP:_output_buffer
    push ax
    call far ptr _sprintf
    add sp, 2
    mov byte ptr console_save_cursor, 1
    mov ax, word ptr console_saved_cursor
    mov word ptr console_cursor_offset, ax
noop_entry3_loop_head_01:
    mov ax, word ptr console_return_cs
    push ax
    mov ax, word ptr console_return_ip
    push ax
    push si
    mov ax, R5HW_VIDEO_MONO_TEXT_SEGMENT ;/* PLATFORM(video): selects a video-memory segment. */
    mov es, ax
; ASMDOC[timers] Block: Literal DGROUP:4BC6h names output_buffer; read formatted bytes until NUL without an explicit
; ASMDOC[timers]   buffer bound.
    mov si, 4BC6h
    mov dh, byte ptr console_text_attribute
noop_entry3_loop_head_02:
    mov dl, [si]
    inc si
    or dl, dl
    jz short noop_entry3_branch_path_02
; ASMDOC[timers] Block: JL is signed: bytes 80h..FFh follow the control path and are ignored unless matching a
; ASMDOC[timers]   recognized control (none do).
    cmp dl, 20h
    jl short noop_entry3_branch_path_04
    mov bx, word ptr console_cursor_offset
    cmp bx, 0FA0h
    jl short noop_entry3_branch_path_01
    call far ptr _console_delete_current_row
    mov bx, 0F00h
    mov word ptr console_cursor_offset, bx
noop_entry3_branch_path_01:
    mov es:[bx], dx
    add word ptr console_cursor_offset, 2
    jmp short noop_entry3_loop_head_02
noop_entry3_branch_path_02:
    cmp byte ptr console_save_cursor, 0
    jz short noop_entry3_branch_path_03
    mov ax, word ptr console_cursor_offset
    mov word ptr console_saved_cursor, ax
noop_entry3_branch_path_03:
    pop si
    retf
; ASMDOC[timers] Block: Control dispatch: LF advances row or scrolls, BS backs up one cell, FF clears, TAB rounds
; ASMDOC[timers]   byte offset to next 16-byte boundary.
noop_entry3_branch_path_04:
    cmp dl, 0Ah
    jnz short noop_entry3_branch_path_06
    cmp word ptr console_cursor_offset, 0F00h
    jle short noop_entry3_branch_path_05
    call far ptr _console_delete_current_row
    mov word ptr console_cursor_offset, 0F00h
    jmp short noop_entry3_loop_head_02
noop_entry3_branch_path_05:
    mov ax, word ptr console_cursor_offset
    mov bx, 0A0h
    div bl
    xor ah, ah
    mul bl
    add ax, 0A0h
    mov word ptr console_cursor_offset, ax
    jmp short noop_entry3_loop_head_02
noop_entry3_branch_path_06:
    cmp dl, 8
    jnz short noop_entry3_branch_path_07
    cmp word ptr console_cursor_offset, 0
    jz short noop_entry3_loop_head_02
    sub word ptr console_cursor_offset, 2
    jmp short noop_entry3_loop_head_02
noop_entry3_branch_path_07:
    cmp dl, 0Ch
    jnz short noop_entry3_branch_path_08
    call far ptr _console_clear_mono_screen
    mov word ptr console_cursor_offset, 0
    jmp noop_entry3_loop_head_02
noop_entry3_branch_path_08:
    cmp dl, 9
    jnz short noop_entry3_branch_path_09
    add word ptr console_cursor_offset, 10h
    and word ptr console_cursor_offset, 0FFF0h
noop_entry3_branch_path_09:
    jmp noop_entry3_loop_head_02
; ASMDOC[timers] Block: Unexported positioned-print fragment: unreachable from public path (prior instruction
; ASMDOC[timers]   unconditionally jumps). It pops return/x/y words and has anomalous SUB SP,2 after sprintf.
noop_entry3_branch_path_10:
    pop ax
    mov word ptr console_return_ip, ax
    pop ax
    mov word ptr console_return_cs, ax
    pop dx
    pop ax
    mov bx, 0A0h
    mul bl
    add ax, dx
    mov word ptr console_cursor_offset, ax
    mov ax, offset DGROUP:_output_buffer
    push ax
    call far ptr _sprintf
    sub sp, 2
    mov byte ptr console_save_cursor, 0
    jmp noop_entry3_loop_head_01
; ASMDOC[timers] Block: Unexported row-selector fragment: clamp signed BP+6 to 0..24 and save row. No incoming
; ASMDOC[timers]   branch/public reference is present.
noop_entry3_branch_path_11:
    push bp
    mov bp, sp
    mov ax, [bp+6]
    cmp ax, 0
    jge short noop_entry3_branch_path_12
    xor ax, ax
noop_entry3_branch_path_12:
    cmp ax, 18h
    jle short noop_entry3_branch_path_13
    mov ax, 18h
noop_entry3_branch_path_13:
    mov byte ptr console_cursor_row, al
    pop bp
    retf
_debug_printf_text endp

; console_set_text_attribute
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 322B4.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: console_text_attribute.
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _nopsub_322B4 (proposed name _console_set_text_attribute): Set console character attribute
; ASMDOC[timers]   from argument low byte.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: SS:[BP+6] word attribute; only AL stored (entry SP+4); DS=DGROUP
; ASMDOC[timers] outputs: console_text_attribute=argument low byte; AX=full argument word.
; ASMDOC[timers] preserves: BP restored; BX CX DX SI DI DS ES flags unmodified.
; ASMDOC[timers] clobbers: AX
; ASMDOC[timers] globals_read:
; ASMDOC[timers] globals_written: console_text_attribute
; ASMDOC[timers] services:
; ASMDOC[timers] reachability: No direct, address-taken, callback or vector reference found; no predecessor
; ASMDOC[timers]   fallthrough (preceding fragment RETF). Preserve complete extent including trailing zero.
; ASMDOC[timers] dead: true (static known-program reachability; never permission to trim extent)
_console_set_text_attribute proc far
    arg_attribute = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_attribute]
    mov     console_text_attribute, al
    pop     bp
    retf

    db 0 ; retained zero fill at this boundary
_console_set_text_attribute endp

S012_TEXT ends
end
