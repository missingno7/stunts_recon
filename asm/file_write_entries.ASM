.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
public aSFileError_0
aSFileError_0     db 37
    db 115
    db 32
    db 70
    db 73
    db 76
    db 69
    db 32
    db 69
    db 82
    db 82
    db 79
    db 82
    db 13
    db 0
    db 0
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:DGROUP
extrn _fatal_error:far
S012_TEXT segment word public 'CODE'
public _file_write_nofatal
public _file_write_fatal
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file write nofatal.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_write_nofatal proc far
    var_fatal = word ptr -4
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 6
    push    ds
    push    si
    push    di
    mov     [bp+var_fatal], 0
    jmp     short _file_write
    db 144
_file_write_nofatal endp
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file write fatal.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_filename (word ptr 6); arg_lenh (word ptr 14); arg_lenl (word ptr 12); arg_srcoff (word
;   ptr 8); arg_srcseg (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, DS, SS; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: aSFileError_0.
; /* PLATFORM(file): calls a DOS file service. */
; -----------------------------------------------------------------------------
_file_write_fatal proc far
    var_errno = word ptr -6
    var_fatal = word ptr -4
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_srcoff = word ptr 8
    arg_srcseg = word ptr 10
    arg_lenl = word ptr 12
    arg_lenh = word ptr 14

    push    bp
    mov     bp, sp
    sub     sp, 6
    push    ds
    push    si
    push    di
    mov     [bp+var_fatal], 1
_file_write:
    mov     dx, [bp+arg_filename]
    xor     cx, cx
    mov     ah, 3Ch
    int     R5HW_DOS_INT ; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT) /* PLATFORM(file): calls a DOS file service. */
    jb      short write_file_fatal_when_below
    mov     [bp+var_filehandle], ax
write_file_fatal_scan_loop:
    cmp     [bp+arg_lenl], 0
    jnz     short write_file_fatal_when_nonzero
    cmp     [bp+arg_lenh], 0
    jz      short write_file_fatal_when_zero
write_file_fatal_when_nonzero:
    mov     cx, 4000h
    mov     ax, [bp+arg_lenl]
    sub     [bp+arg_lenl], cx
    sbb     [bp+arg_lenh], 0
    jnb     short file_fatal_when_not_below
    mov     cx, [bp+arg_lenl]
    mov     [bp+arg_lenl], 0
    mov     [bp+arg_lenh], 0
    add     cx, 4000h
file_fatal_when_not_below:
    mov     ds, [bp+arg_srcseg]
    mov     dx, [bp+arg_srcoff]
    mov     bx, [bp+var_filehandle]
    mov     ah, 40h
    int     R5HW_DOS_INT ; DOS - 2+ - WRITE TO FILE WITH HANDLE /* PLATFORM(file): calls a DOS file service. */
    jb      short write_file_fatal_when_below
    cmp     ax, cx
    jnz     short file_fatal_when_nonzero_2
    add     [bp+arg_srcseg], 400h
    jmp     short write_file_fatal_scan_loop
file_fatal_when_nonzero_2:
    mov     ax, 1
write_file_fatal_when_below:
    mov     [bp+var_errno], ax
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     R5HW_DOS_INT ; DOS - 2+ - CLOSE A FILE WITH HANDLE /* PLATFORM(file): calls a DOS file service. */
    mov     ax, ss
    mov     ds, ax
    mov     dx, [bp+arg_filename]
    mov     ah, 41h
    int     R5HW_DOS_INT ; DOS - 2+ - DELETE A FILE (UNLINK) /* PLATFORM(file): calls a DOS file service. */
    mov     ax, [bp+var_errno]
    cmp     [bp+var_fatal], 0
    jnz     short file_fatal_return_if_nonzero
    mov     ax, ss
    mov     ds, ax
    mov     ax, offset DGROUP:aSFileError_0; "%s FILE ERROR\r"
    push    [bp+arg_filename]
    push    ax
    call    far ptr _fatal_error
write_file_fatal_when_zero:
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     R5HW_DOS_INT ; DOS - 2+ - CLOSE A FILE WITH HANDLE /* PLATFORM(file): calls a DOS file service. */
    xor     ax, ax
file_fatal_return_if_nonzero:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
_file_write_fatal endp
S012_TEXT ends
end
