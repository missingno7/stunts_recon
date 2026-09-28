.8086
INCLUDE include\platform_hw.inc
DSEG segment byte public 'STUNTSD'
DSEG ends
_DATA segment word public 'DATA'
console_cursor_row db 0
console_save_cursor db 0
console_text_attribute db 7
db 0
console_saved_cursor dw 3840
console_cursor_offset dw 0
console_return_ip dw 0
console_return_cs dw 0
_output_buffer db 120 dup (0)
_DATA ends
DGROUP group _DATA,DSEG
assume ds:DGROUP
extrn _sprintf:far
public _nopsub_3215A
public _nopsub_3216C
public _debug_printf_text
public _nopsub_322B4
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP

; nopsub_3215A
assume cs:S012_TEXT
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 3215A.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, CX, DI, ES; balanced explicit saves/restores observed: DI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): selects a video-memory segment. */
; -----------------------------------------------------------------------------
_nopsub_3215A proc far
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
_nopsub_3215A endp

; nopsub_3216C
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 3216C.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, CX, SI, DI, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: console_cursor_row.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_3216C proc far
     r = byte ptr 0

    push    si
    push    di
    push    ds
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
_nopsub_3216C endp

; nopsub_3219D
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by debug printf text.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, DX, SI, BP, SP, ES; balanced explicit saves/restores observed: AX, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: console_cursor_offset, console_return_cs, console_return_ip, console_save_cursor, console_saved_cursor, console_text_attribute.
; Globals written: console_cursor_offset, console_cursor_row, console_return_cs, console_return_ip, console_save_cursor, console_saved_cursor.
; Global addresses taken: _output_buffer.
; /* PLATFORM(video): selects a video-memory segment. */
; -----------------------------------------------------------------------------
_debug_printf_text proc far
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
    mov si, 4BC6h
    mov dh, byte ptr console_text_attribute
noop_entry3_loop_head_02:
    mov dl, byte ptr [si]
    inc si
    or dl, dl
    jz short noop_entry3_branch_path_02
    cmp dl, 20h
    jl short noop_entry3_branch_path_04
    mov bx, word ptr console_cursor_offset
    cmp bx, 0FA0h
    jl short noop_entry3_branch_path_01
    call far ptr _nopsub_3216C
    mov bx, 0F00h
    mov word ptr console_cursor_offset, bx
noop_entry3_branch_path_01:
    mov word ptr es:[bx], dx
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
noop_entry3_branch_path_04:
    cmp dl, 0Ah
    jnz short noop_entry3_branch_path_06
    cmp word ptr console_cursor_offset, 0F00h
    jle short noop_entry3_branch_path_05
    call far ptr _nopsub_3216C
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
    call far ptr _nopsub_3215A
    mov word ptr console_cursor_offset, 0
    jmp noop_entry3_loop_head_02
noop_entry3_branch_path_08:
    cmp dl, 9
    jnz short noop_entry3_branch_path_09
    add word ptr console_cursor_offset, 10h
    and word ptr console_cursor_offset, 0FFF0h
noop_entry3_branch_path_09:
    jmp noop_entry3_loop_head_02
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
noop_entry3_branch_path_11:
    push bp
    mov bp, sp
    mov ax, word ptr [bp+6]
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

; nopsub_322B4
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 322B4.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: console_text_attribute.
; -----------------------------------------------------------------------------
_nopsub_322B4 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_0]
    mov     console_text_attribute, al
    pop     bp
    retf

    db 0
_nopsub_322B4 endp

S012_TEXT ends
end
