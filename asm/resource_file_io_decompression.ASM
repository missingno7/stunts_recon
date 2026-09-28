.8086
INCLUDE include\platform_hw.inc
name asm012_133840
DSEG segment byte public 'STUNTSD'
DSEG ends
_DATA segment word public 'DATA'
_aSFileError db '%s FILE ERROR',13,0
_aSNotPackedFile db '%s NOT PACKED FILE',13,0
_aSInvalidPackTy db '%s INVALID PACK TYPE',13,0,0
aLocateshape4_4sShapeNotF db 'locateshape - %-4.4s SHAPE NOT FOUND',13,10
aLocatesound4_4sSoundNotF db 'locatesound - %-4.4s SOUND NOT FOUND',13,10
_DATA ends
DGROUP group _DATA,DSEG
assume ds:DGROUP
extrn _fatal_error:far
extrn _copy_paras_reverse:far
extrn _mmgr_get_chunk_by_name:far
extrn _file_paras:far
extrn _mmgr_alloc_pages:far
extrn _file_decomp_paras:far
extrn _mmgr_resize_memory:far
extrn _file_decomp_vle:far
public _file_read, _file_read_nofatal, _file_read_fatal
public _file_decomp_rle, _file_decomp_rle_single, _file_decomp_rle_seq
public _file_load_binary
public _file_load_binary_nofatal
public _file_decomp, _file_decomp_nofatal, _file_decomp_fatal
public _locate_shape_nofatal
public _locate_shape_fatal
public _locate_sound_fatal
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP

; file_read
assume cs:S012_TEXT
; -----------------------------------------------------------------------------
; Purpose: Read file data through the DOS handle API.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_fatal (word ptr 12).
; Register ABI: touches AX, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_read proc far
    var_fatal = word ptr -8
     s = byte ptr 0
     r = byte ptr 2
    arg_fatal = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    mov     ax, [bp+arg_fatal]
    mov     [bp+var_fatal], ax
    jmp     short read_body
    ; align 2
    db 144

_file_read endp
; -----------------------------------------------------------------------------
; Purpose: Read file data and return failure without raising a fatal error.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_read_nofatal proc far
    var_fatal = word ptr -8
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    mov     [bp+var_fatal], 0
    jmp     short read_body
    db 144

_file_read_nofatal endp
; -----------------------------------------------------------------------------
; Purpose: Read file data and route failures to the fatal-error handler.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_dstoff (word ptr 8); arg_dstseg (word ptr 10); arg_filename (word ptr 6).
; Register ABI: touches AX, BX, CX, DX, BP, SP, DS, SS; balanced explicit saves/restores observed: DS; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _aSFileError.
; /* PLATFORM(file): calls a DOS file service. */
; -----------------------------------------------------------------------------
_file_read_fatal proc far
    var_fatal = word ptr -8
    var_curseg = word ptr -6
    var_curoff = word ptr -4
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_dstoff = word ptr 8
    arg_dstseg = word ptr 10

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    mov     [bp+var_fatal], 1
read_body:
    mov     ax, [bp+arg_dstoff]
    mov     [bp+var_curoff], ax
    mov     ax, [bp+arg_dstseg]
    mov     [bp+var_curseg], ax
    mov     dx, [bp+arg_filename]
    xor     al, al
    mov     ah, 3Dh
    int     R5HW_DOS_INT ; DOS - 2+ - OPEN DISK FILE WITH HANDLE /* PLATFORM(file): calls a DOS file service. */
    jb      short error
    mov     [bp+var_filehandle], ax
readloop:
    mov     ds, [bp+var_curseg]
    mov     dx, [bp+var_curoff]
    mov     cx, 4000h
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Fh
    int     R5HW_DOS_INT ; DOS - 2+ - READ FROM FILE WITH HANDLE /* PLATFORM(file): calls a DOS file service. */
    jb      short error
    add     [bp+var_curseg], 400h
    cmp     ax, 4000h
    jz      short readloop
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     R5HW_DOS_INT ; DOS - 2+ - CLOSE A FILE WITH HANDLE /* PLATFORM(file): calls a DOS file service. */
    mov     ax, [bp+arg_dstoff]
    mov     dx, [bp+arg_dstseg]
    pop     ds
    mov     sp, bp
    pop     bp
    retf
error:
    cmp     [bp+var_fatal], 0
    jnz     short fatal
    xor     ax, ax
    xor     dx, dx
    pop     ds
    mov     sp, bp
    pop     bp
    retf
fatal:
    mov     ax, offset DGROUP:_aSFileError; "%s FILE ERROR\r"
    mov     bx, ss
    mov     ds, bx
    push    [bp+arg_filename]
    push    ax
    call    far ptr _fatal_error

_file_read_fatal endp

; file_decomp_rle
assume cs:S012_TEXT
; -----------------------------------------------------------------------------
; Purpose: Expand an RLE-compressed resource stream.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: arg_dstoff (word ptr 8); arg_dstseg (word ptr 10); arg_srcoff (word ptr 6); arg_srcseg
;   (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_decomp_rle proc far
    var_lenlo = word ptr -26
    var_lenhi = word ptr -24
    var_16 = word ptr -22
    var_14 = word ptr -20
    var_esclen = word ptr -18
     s = byte ptr 0
     r = byte ptr 2
    arg_srcoff = word ptr 6
    arg_srcseg = word ptr 8
    arg_dstoff = word ptr 12
    arg_dstseg = word ptr 14

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    sub     sp, 116h
    cld
    mov     ax, ds
    mov     es, ax
    mov     si, [bp+arg_srcoff]
    mov     ds, [bp+arg_srcseg]
    mov     ax, [si+1]
    mov     [bp+var_lenlo], ax
    mov     al, [si+3]
    xor     ah, ah
    mov     [bp+var_lenhi], ax
    add     si, 4           ; Skip header + unused byte
    mov     cx, 8
    lea     di, [bp+var_16] ; Read 8 words
    rep movsw
    mov     si, [bp+arg_srcoff]; Reset source pos
    add     si, 9
    mov     dx, [bp+var_esclen]

    and     dx, 7Fh         ; Escape codes length mask

    add     si, dx
    mov     [bp+arg_srcoff], si; Skip escape codes
    cmp     byte ptr [bp+var_esclen], 80h ; ''; Skip seq pass flag
    ja      short skip_seq_pass
    call near ptr _file_decomp_rle_seq
    mov     [bp+var_16], ax
    mov     [bp+var_14], dx
    mov     si, ax          ; paras = len / 16
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    test    si, 0Fh
    jz      short no_remainder
    inc     ax
no_remainder:
    push    ax              ; paras
    mov     bx, [bp+arg_dstseg]
    sub     bx, ax
    add     bx, [bp+arg_dstoff]
    mov     [bp+arg_srcseg], bx
    push    bx
    push    [bp+arg_dstoff]
    call far ptr _copy_paras_reverse
    add     sp, 6
    xor     si, si
    mov     [bp+arg_srcoff], si
skip_seq_pass:
    call near ptr _file_decomp_rle_single
    mov     ax, [bp+var_lenlo]; Return length
    mov     dx, [bp+var_lenhi]
    add     sp, 116h
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf

_file_decomp_rle endp
; -----------------------------------------------------------------------------
; Purpose: Expand one RLE-compressed resource block.
; ABI: NEAR entry; source return form: RET mnemonic for NEAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, DS, ES, SS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_decomp_rle_single proc near
    mov     cx, 80h ; ''
    lea     di, [bp-11Ch]
    mov     ax, ss
    mov     es, ax
    mov     ds, ax
    xor     ax, ax
    rep stosw
    mov     cl, [bp-12h]

    and     cx, 7Fh

    lea     si, [bp-11h]
    lea     di, [bp-11Ch]
    xor     ax, ax
    xor     bh, bh
rle_single_loop_head_01:
    inc     ax
    mov     bl, [si]
    mov     [bx+di], al
    inc     si
    loop    rle_single_loop_head_01
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     es, word ptr [bp+0Ch]
    xor     di, di
    mov     dx, [bp-16h]
    mov     ax, [bp-14h]
    mov     [bp-1Ch], ax
rle_single_loop_head_02:
    cmp     si, 8000h
    jbe     short rle_single_loop_head_03
    jmp     short rle_single_join_01
    db 144
rle_single_loop_head_03:
    cmp     di, 8000h
    jbe     short rle_single_loop_head_04
    jmp     short rle_single_join_02
    ; align 2
    db 144
rle_single_loop_head_04:
    lodsb
    xor     ah, ah
    lea     bx, [bp-11Ch]
    add     bx, ax
    mov     cl, ss:[bx]
    test    cl, 0FFh
    jnz     short rle_single_branch_path_01
    stosb
    dec     dx
    jnz     short rle_single_loop_head_02
    cmp     word ptr [bp-1Ch], 0
    jz      short rle_single_branch_path_02
    dec     word ptr [bp-1Ch]
    jmp     short rle_single_loop_head_02
rle_single_branch_path_01:
    xor     ch, ch
    cmp     cl, 1
    jz      short rle_single_branch_path_03
    cmp     cl, 3
    jz      short rle_single_branch_path_04
    dec     cx
    lodsb
    rep stosb
    sub     dx, 2
rle_single_loop_head_05:
    mov     ax, [bp-1Ch]
    sbb     ax, 0
    mov     [bp-1Ch], ax
    or      ax, dx
    jz      short rle_single_branch_path_02
    jmp     short rle_single_loop_head_02
rle_single_branch_path_02:
    mov     dx, es
    sub     dx, [bp+0Ch]
    mov     ax, 10h
    mul     dx
    add     ax, di
    adc     dx, 0
    retn
rle_single_branch_path_03:
    lodsb
    mov     cl, al
    lodsb
    rep stosb
    sub     dx, 3
    jmp     short rle_single_loop_head_05
rle_single_branch_path_04:
    lodsb
    mov     cl, al
    lodsb
    mov     ch, al
    lodsb
    rep stosb
    sub     dx, 4
    jmp     short rle_single_loop_head_05
rle_single_join_01:
    sub     si, 8000h
    mov     ax, ds
    add     ax, 800h
    mov     ds, ax
    jmp     rle_single_loop_head_03
rle_single_join_02:
    sub     di, 8000h
    mov     ax, es
    add     ax, 800h
    mov     es, ax
    jmp     rle_single_loop_head_04

_file_decomp_rle_single endp
; -----------------------------------------------------------------------------
; Purpose: Expand a sequence of RLE-compressed resource blocks.
; ABI: NEAR entry; source return form: RET mnemonic for NEAR entry; stack arguments: arg_srcoff (word ptr 6); arg_srcseg (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, DS, ES; balanced explicit saves/restores observed: CX, DS, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_decomp_rle_seq proc near
    cmp     byte ptr [bp-12h], 1; _file_decomp_rle::var_esclen
    jnz     short has_codes
    retn
has_codes:
    mov     ds, word ptr [bp+8]; _file_decomp_rle::arg_srcseg
    mov     si, [bp+6]      ; _file_decomp_rle::arg_srcoff
    mov     es, word ptr [bp+0Ch]; _file_decomp_rle::var_dstoff
    xor     di, di
    mov     bx, [bp-16h]
    mov     ax, [bp-14h]
    mov     [bp-1Ch], ax
    mov     ah, [bp-10h]
rle_sequence_loop_head_01:
    cmp     si, 8000h
    ja      short rle_sequence_branch_path_01
rle_sequence_loop_head_02:
    cmp     di, 8000h
    ja      short rle_sequence_branch_path_02
rle_sequence_loop_head_03:
    lodsb
    cmp     al, ah
    jz      short rle_sequence_branch_path_03
    stosb
    dec     bx
    jnz     short rle_sequence_loop_head_01
    cmp     word ptr [bp-1Ch], 0
    jz      short rle_sequence_loop_head_04
    dec     word ptr [bp-1Ch]
    jmp     short rle_sequence_loop_head_01
rle_sequence_branch_path_01:
    sub     si, 8000h
    mov     dx, ds
    add     dx, 800h
    mov     ds, dx
    jmp     short rle_sequence_loop_head_02
rle_sequence_branch_path_02:
    sub     di, 8000h
    mov     dx, es
    add     dx, 800h
    mov     es, dx
    jmp     short rle_sequence_loop_head_03
rle_sequence_loop_head_04:
    mov     dx, es
    sub     dx, [bp+0Ch]
    mov     ax, 10h
    mul     dx
    add     ax, di
    adc     dx, 0
    retn
rle_sequence_branch_path_03:
    mov     dx, di
rle_sequence_loop_head_05:
    lodsb
    cmp     al, ah
    jz      short rle_sequence_branch_path_04
    stosb
    dec     bx
    jnz     short rle_sequence_loop_head_05
    dec     word ptr [bp-1Ch]
    jmp     short rle_sequence_loop_head_05
rle_sequence_branch_path_04:
    lodsb
    push    si
    push    ds
    mov     cx, es
    mov     ds, cx
    mov     si, dx
    mov     cx, di
    sub     cx, dx
    mov     dl, al
    dec     dl
rle_sequence_loop_head_06:
    push    cx
    push    si
    push    ds
    rep movsb
    pop     ds
    pop     si
    pop     cx
    dec     dl
    jnz     short rle_sequence_loop_head_06
    pop     ds
    pop     si
    sub     bx, 3
    sbb     word ptr [bp-1Ch], 0
    mov     dx, [bp-1Ch]
    or      dx, bx
    jz      short rle_sequence_loop_head_04
    jmp     rle_sequence_loop_head_01

_file_decomp_rle_seq endp

; file_load_binary_group
assume cs:S012_TEXT
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file load binary.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_load_binary proc far
    push bp
    mov bp, sp
    sub sp, 2
    mov ax, [bp+8]
    mov [bp-2], ax
    jmp short common_entry
    nop
_file_load_binary endp
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file load binary nofatal.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_load_binary_nofatal proc far
    push bp
    mov bp, sp
    sub sp, 2
    mov word ptr [bp-2], 0
    jmp short common_entry
    nop
fatal_entry:
    push bp
    mov bp, sp
    sub sp, 2
    mov word ptr [bp-2], 1
common_entry:
    push word ptr [bp+6]
    call far ptr _mmgr_get_chunk_by_name
    add sp, 2
    or dx, dx
    jnz load_complete
    push word ptr [bp-2]
    push word ptr [bp+6]
    call far ptr _file_paras
    add sp, 4
    or ax, ax
    jz empty_file
    push ax
    push word ptr [bp+6]
    call far ptr _mmgr_alloc_pages
    add sp, 4
    push word ptr [bp-2]
    push dx
    push ax
    push word ptr [bp+6]
    call far ptr _file_read
    add sp, 8
load_complete:
    mov sp, bp
    pop bp
    retf
empty_file:
    xor dx, dx
    jmp short load_complete
_file_load_binary_nofatal endp

; file_decomp_group
; -----------------------------------------------------------------------------
; Purpose: Dispatch resource decompression.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_fatal (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_decomp proc far
    var_fatal = word ptr -12
     s = byte ptr 0
     r = byte ptr 2
    arg_fatal = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    si
    push    di
    mov     ax, [bp+arg_fatal]
    mov     [bp+var_fatal], ax
    jmp     short decomp_body
    db 144
_file_decomp endp
; -----------------------------------------------------------------------------
; Purpose: Dispatch resource decompression with nonfatal error handling.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_decomp_nofatal proc far
    var_fatal = word ptr -12
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    si
    push    di
    mov     [bp+var_fatal], 0
    jmp     short decomp_body
    db 144
_file_decomp_nofatal endp
; -----------------------------------------------------------------------------
; Purpose: Dispatch resource decompression with fatal error handling.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_6 (word ptr 12); arg_filename (word ptr 6).
; Register ABI: touches AX, BX, DX, SI, DI, BP, SP, CS, DS, SS; balanced explicit saves/restores observed: DI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _aSInvalidPackTy.
; -----------------------------------------------------------------------------
_file_decomp_fatal proc far
    var_fatal = word ptr -12
    var_nextsrcseg = word ptr -10
    var_passes = word ptr -8
    var_decomp_paras = word ptr -6
    var_dstseg = word ptr -4
    var_dstoff = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    si
    push    di
    mov     [bp+var_fatal], 1
decomp_body:
    push    [bp+arg_filename]
    call    _mmgr_get_chunk_by_name
    add     sp, 2
    or      dx, dx
    jz      short fd_not_found
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
decomp_fatal_loop_head_01:
    jmp     decomp_fatal_join_01
fd_not_found:
    push    [bp+var_fatal]
    push    [bp+arg_filename]
    call    _file_decomp_paras
    add     sp, 4
    or      ax, ax
    jz      short decomp_fatal_loop_head_01
    add     ax, 4
    mov     [bp+var_decomp_paras], ax
    push    ax
    push    [bp+arg_filename]
    call    _mmgr_alloc_pages
    add     sp, 4
    mov     [bp+var_dstoff], ax
    mov     [bp+var_dstseg], dx
    push    [bp+var_fatal]
    push    [bp+arg_filename]
    call    _file_paras
    add     sp, 4
    or      ax, ax
    jz      short decomp_fatal_loop_head_01
    mov     dx, [bp+var_decomp_paras]
    sub     dx, ax
    add     dx, [bp+var_dstseg]
    push    [bp+var_fatal]
    push    dx
    push    [bp+var_dstoff]
    push    [bp+arg_filename]
    call    _file_read
    add     sp, 8
    or      dx, dx
    jz      short decomp_fatal_loop_head_01
    mov     [bp+var_passes], 1
    mov     ds, dx
    mov     si, ax
    mov     al, [si]
    test    al, 80h
    jz      short decompress_subfile
    and     ax, 7Fh
    mov     [bp+var_passes], ax
    add     si, 4
decompress_subfile:
    push    [bp+var_decomp_paras]
    push    [bp+var_dstseg]
    push    [bp+var_dstoff]
    push    ds
    mov     bl, [si]
    dec     bl
    jl      short decomp_fatal_branch_path_02
    cmp     bl, 2
    jge     short decomp_fatal_branch_path_02
    push    si
    xor     bh, bh
    shl     bx, 1
    mov     ax, seg dseg
    mov     ds, ax
    shl     bx, 1
    call    dword ptr cs:compression_type[bx]
    add     sp, 0Ah
    dec     [bp+var_passes]
    jle     short fd_done
    mov     si, ax
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    test    si, 0Fh
    jz      short decomp_fatal_branch_path_01
    inc     ax
decomp_fatal_branch_path_01:
    push    ax
    mov     bx, [bp+var_decomp_paras]
    sub     bx, ax
    add     bx, [bp+var_dstseg]
    mov     [bp+var_nextsrcseg], bx
    push    bx
    push    [bp+var_dstseg]
    call    _copy_paras_reverse
    add     sp, 6
    mov     ds, [bp+var_nextsrcseg]
    xor     si, si
    jmp     short decompress_subfile
compression_type     dd _file_decomp_rle
    dd _file_decomp_vle
decomp_fatal_branch_path_02:
    add     sp, 8
decomp_fatal_join_01:
    cmp     [bp+var_fatal], 0
    jnz     short decomp_fatal_error_path_01
    xor     ax, ax
    xor     dx, dx
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
decomp_fatal_error_path_01:
    mov     bx, ss
    mov     ds, bx
    push    [bp+arg_filename]
    mov     ax, offset DGROUP:_aSInvalidPackTy; "%s INVALID PACK TYPE\r"
    push    ax
    call    far ptr _fatal_error
fd_done:
    mov     ax, [bp+var_decomp_paras]
    sub     ax, 4
    push    ax
    push    [bp+var_dstseg]
    push    [bp+var_dstoff]
    call    _mmgr_resize_memory
    add     sp, 6
    mov     ax, [bp+var_dstoff]
    mov     dx, [bp+var_dstseg]
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    si
    push    di
    mov     ax, [bp+arg_6]
    mov     [bp+var_fatal], ax
    jmp     short decomp_fatal_join_02
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    si
    push    di
    mov     [bp+var_fatal], 0
    jmp     short decomp_fatal_join_02
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    si
    push    di
    mov     [bp+var_fatal], 1
decomp_fatal_join_02:
    push    [bp+var_fatal]
    push    [bp+arg_filename]
    call    _file_decomp_fatal
    add     sp, 4
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf

_file_decomp_fatal endp

; locate_entries
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by locate shape nofatal.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches DX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_locate_shape_nofatal proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    xor     dx, dx
    jmp     short _alt_locate_resource
    db 144
_locate_shape_nofatal endp
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by locate shape fatal.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches DX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_locate_shape_fatal proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     dx, 1
    jmp     short _alt_locate_resource
    db 144
_locate_shape_fatal endp
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by locate sound fatal.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: aLocateshape4_4sShapeNotF, aLocatesound4_4sSoundNotF.
; /* PLATFORM(dos): terminates the DOS process. */
; -----------------------------------------------------------------------------
_locate_sound_fatal proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     dx, 2
_alt_locate_resource:
    cld
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, seg dseg
    mov     es, ax
    mov     di, [bp+arg_4]
    mov     cx, 4
    mov     bx, 0
find_sound_fatal_loop_head_01:
    cmp     byte ptr es:[bx+di], 0
    jz      short find_sound_fatal_loop_head_05
    inc     bx
    loop    find_sound_fatal_loop_head_01
find_sound_fatal_loop_head_02:
    mov     ax, [si+4]
    or      ax, ax
    jz      short _end_of_locate
    add     si, 6
    mov     bx, si
find_sound_fatal_loop_head_03:
    mov     si, bx
    mov     di, [bp+arg_4]
    mov     cx, 4
find_sound_fatal_loop_head_04:
    cmpsb
    jnz     short find_sound_fatal_branch_path_01
    loop    find_sound_fatal_loop_head_04
    jmp     short _found_resource
    ; align 2
    db 144
find_sound_fatal_branch_path_01:
    cmp     byte ptr [si-1], 0
    jnz     short find_sound_fatal_branch_path_02
    cmp     byte ptr es:[di-1], 20h ; ' '
    jz      short _found_resource
find_sound_fatal_branch_path_02:
    add     bx, 4
    dec     ax
    jge     short find_sound_fatal_loop_head_03
_end_of_locate:
    cmp     dx, 1
    jl      short find_sound_fatal_branch_path_04
    jg      short find_sound_fatal_branch_path_03
    mov     dx, offset DGROUP:aLocateshape4_4sShapeNotF; "locateshape - %-4.4s SHAPE NOT FOUND\r\n"
    jmp     short find_sound_fatal_error_path_01
    ; align 2
    db 144
find_sound_fatal_branch_path_03:
    mov     dx, offset DGROUP:aLocatesound4_4sSoundNotF; "locatesound - %-4.4s SOUND NOT FOUND\r\n"
find_sound_fatal_error_path_01:
    mov     ax, seg dseg
    mov     ds, ax
    push    [bp+arg_4]
    push    dx
    call    far ptr _fatal_error
    int     R5HW_DOS_TERMINATE_INT ; DOS - PROGRAM TERMINATION /* PLATFORM(dos): terminates the DOS process. */
find_sound_fatal_branch_path_04:
    xor     ax, ax
    jmp     short find_sound_fatal_join_01
    ; align 2
    db 144
find_sound_fatal_loop_head_05:
    mov     byte ptr es:[bx+di], 20h ; ' '
    inc     bx
    loop    find_sound_fatal_loop_head_05
    jmp     short find_sound_fatal_loop_head_02
_found_resource:
    mov     si, [bp+arg_0]
    mov     ax, [si+4]
    shl     ax, 1
    shl     ax, 1
    add     bx, ax
    shl     ax, 1
    add     ax, 6
    mov     cx, ds
    xor     dx, dx
    shl     cx, 1
    rcl     dx, 1
    shl     cx, 1
    rcl     dx, 1
    shl     cx, 1
    rcl     dx, 1
    shl     cx, 1
    rcl     dx, 1
    add     ax, cx
    adc     dx, 0
    add     ax, [bx]
    adc     dx, [bx+2]
    mov     bx, ax

    and     bx, 0Fh

    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    mov     dx, ax
    mov     ax, bx
find_sound_fatal_join_01:
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
_locate_sound_fatal endp

S012_TEXT ends
end
