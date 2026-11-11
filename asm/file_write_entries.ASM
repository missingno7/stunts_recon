; ASMDOC Module: DOS far-source streaming writes and their two inverted historical policy names.
; ASMDOC Owner file_write_entries; original source asm/file_write_entries.ASM.
; ASMDOC Routine inventory: _file_write_nofatal, _file_write_fatal
; ASMDOC All BP stack offsets below follow PUSH BP / MOV BP,SP; data spans are caller-owned.
; ASMDOC Proposed module name describes both policies without perpetuating the inverted fatal/nofatal spelling.
; ASMDOC No self-modifying code or indirect callback. Shared _file_write is an interior implementation label,
; not public.
.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
public aSFileError_0
; ASMDOC Exported DATA aSFileError_0: error format, CR, NUL and final alignment NUL; never a callable entry.
aSFileError_0 db '%s FILE ERROR', 0Dh, 00h, 00h
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
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; no named stack arguments referenced.
; Register ABI: touches SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_write_nofatal
; ASMDOC purpose: Create/truncate and stream a far source to disk; policy 0 calls fatal_error on failure
; (existing name is inverted).
; ASMDOC convention: FAR caller-clean stack; BP offsets after PUSH BP / MOV BP,SP (entry SP offsets are BP
; offsets minus 2); RETF via shared
; ASMDOC body where applicable.
; ASMDOC inputs: BP+6 near filename in entry DS (normally DGROUP=SS); BP+8 source offset; BP+10 source segment;
; BP+12 length low word; BP+14
; ASMDOC length high word; Caller argument storage is writable: source segment and both length words are mutated
; ASMDOC outputs: AX=0 on successful write/close (close status ignored); policy-1 errors return DOS AX or
; synthetic 1 for a short write;
; ASMDOC Deletes filename on error; policy 0 calls fatal_error after cleanup; partial source progress and
; modified stack arguments remain;
; ASMDOC Create failure still closes uninitialized BP-2 handle then attempts deletion of filename
; ASMDOC preserves: BP DS SI DI SP restored on returning paths
; ASMDOC clobbers: AX BX CX DX arithmetic flags; DS transiently source/SS; fatal_error effects if it
; unexpectedly returns
; ASMDOC globals_read: aSFileError_0 format bytes on policy-0 error
; ASMDOC globals_written: none directly; see transitive effects stated in services
; ASMDOC services: DOS INT 21h AH=3Ch CX=0 create/truncate; AH=40h write <=0x4000; AH=3Eh close; AH=41h unlink
; on error;
; ASMDOC fatal_error(format,filename) only when BP-4==0; named callee effects/nonreturn assumption apply
; ASMDOC dead: True
; ASMDOC Public name from existing plan: _file_write_nofatal (unchanged).
_file_write_nofatal proc far
    var_fatal = word ptr -4
    push    bp
    mov     bp, sp
    sub     sp, 6
    push    ds
    push    si
    push    di
    mov     [bp+var_fatal], 0
    jmp     short _file_write
    nop ; retained code pad at an even offset; EVEN would emit no byte
_file_write_nofatal endp
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file write fatal.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_filename (word ptr 6);
; arg_lenh (word ptr 14); arg_lenl (word ptr 12); arg_srcoff (word
;   ptr 8); arg_srcseg (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, DS, SS; balanced explicit saves/restores observed: DI,
; DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: aSFileError_0.
; /* PLATFORM(file): calls a DOS file service. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_write_fatal
; ASMDOC purpose: Create/truncate and stream a far source to disk; policy 1 returns an error on failure
; (existing name is inverted).
; ASMDOC convention: FAR caller-clean stack; BP offsets after PUSH BP / MOV BP,SP (entry SP offsets are BP
; offsets minus 2); RETF via shared
; ASMDOC body where applicable.
; ASMDOC inputs: BP+6 near filename in entry DS (normally DGROUP=SS); BP+8 source offset; BP+10 source segment;
; BP+12 length low word; BP+14
; ASMDOC length high word; Caller argument storage is writable: source segment and both length words are mutated
; ASMDOC outputs: AX=0 on successful write/close (close status ignored); policy-1 errors return DOS AX or
; synthetic 1 for a short write;
; ASMDOC Deletes filename on error; policy 0 calls fatal_error after cleanup; partial source progress and
; modified stack arguments remain;
; ASMDOC Create failure still closes uninitialized BP-2 handle then attempts deletion of filename
; ASMDOC preserves: BP DS SI DI SP restored on returning paths
; ASMDOC clobbers: AX BX CX DX arithmetic flags; DS transiently source/SS; fatal_error effects if it
; unexpectedly returns
; ASMDOC globals_read: aSFileError_0 format bytes on policy-0 error
; ASMDOC globals_written: none directly; see transitive effects stated in services
; ASMDOC services: DOS INT 21h AH=3Ch CX=0 create/truncate; AH=40h write <=0x4000; AH=3Eh close; AH=41h unlink
; on error;
; ASMDOC fatal_error(format,filename) only when BP-4==0; named callee effects/nonreturn assumption apply
; ASMDOC dead: False
; ASMDOC Public name from existing plan: _file_write_fatal (unchanged).
_file_write_fatal proc far
    var_errno = word ptr -6
    var_fatal = word ptr -4
    var_filehandle = word ptr -2
     s = byte ptr 0
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
; ASMDOC Shared body: names are inverted by the later policy test; do not infer error policy from the existing
; export spelling.
_file_write:
    mov     dx, [bp+arg_filename]
    xor     cx, cx
    mov     ah, 3Ch
    int     R5HW_DOS_INT
; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT) /* PLATFORM(file): calls a DOS file service. */
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
; ASMDOC Both create failure and write failure reach cleanup; BP-2 was initialized only after a successful create.
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
; ASMDOC Machine-code policy is decisive: nonzero returns AX; zero falls through to fatal_error.
    cmp     [bp+var_fatal], 0
    jnz     short file_fatal_return_if_nonzero
    mov     ax, ss
    mov     ds, ax
    mov     ax, offset DGROUP:aSFileError_0; "%s FILE ERROR\r"
    push    [bp+arg_filename]
    push    ax
    call    far ptr _fatal_error
; ASMDOC Success path ignores close status and forces AX=0.
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
