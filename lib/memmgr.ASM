; ASMDOC Module: resource_memory_manager. Two-ended resource arena in 16-byte paragraphs. MEMCHUNK is 18 bytes: 12
; ASMDOC name bytes, size word +0Ch, segment word +0Eh, state word +10h. Low live state is 2; high cached state is
; ASMDOC 1; zero is vacant. Full table and diagnostics remain data.
; ASMDOC Static documentation candidate only. Original comments retained verbatim; these contracts correct generic
; ASMDOC earlier annotations.
; ASMDOC No exhaustive dynamic call graph. dead=true means no retained source reference in this bounded audit, not
; ASMDOC permission to remove bytes.
.8086
INCLUDE include\hw.inc
name asm012_135290
MEMCHUNK struc
resname db 12 dup (?)
ressize dw ?
resofs dw ?
res_flags dw ?
MEMCHUNK ends
DSEG segment byte public 'STUNTSD'
DSEG ends
_DATA segment word public 'DATA'
_DATA ends
DGROUP group _DATA, DSEG
assume ds:DGROUP
extrn _abortmessage:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _mmgr_alloc_resmem
public _initmemory
public _mmgr_init_video_reserve
public _initmemsize
public _poolmemory
public _lockedmemory
public _unlockedmemory
public _posblockmove
public _negblockmove
public _mmgr_path_to_name
public _reservememory
public _mmgr_find_free
public _checkmodule
public _ifmodule
public _freememory
public _freetotop
public _purgememory
public _modulesize
public _resizememory
public _packmemory
public _nomemerr, _expanderr, _exmemerr, _resources
; -----------------------------------------------------------------------------
; Purpose: Allocate a resource-memory slot and associate the requested region.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _resendptr2, _resmem_base_seg, _resmem_end_seg, _resptr1.
; Globals written: _pspofs, _pspseg, _resendptr1, _resmem_base_seg, _resmem_end_seg, _resptr2.
;                                          
;                                                               
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_alloc_resmem (_mmgr_alloc_resmem): Initialize resource arena, optionally allocating DOS
; ASMDOC block and resizing it toward the supplied exclusive segment boundary.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 u16 desired exclusive upper segment; DS=DGROUP; descriptor sentinels already established
; ASMDOC outputs: Arena base/end segments and boundary descriptors initialized; resptr2=resptr1;
; ASMDOC resendptr1=resendptr2; interior states cleared; No defined scalar result
; ASMDOC preserves: SI; DI; BP; DS if DOS preserves it
; ASMDOC clobbers: AX; BX; ES; DOS-dependent effects/flags
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP;
; ASMDOC _resmem_end_seg; _resmem_base_seg
; ASMDOC globals_written: _pspofs; _pspseg; _resmem_base_seg; _resmem_end_seg; _resptr2; _resendptr1; interior
; ASMDOC MEMCHUNK state words
; ASMDOC services: INT21h AX=3000h DOS version; INT21h AH=62h PSP (DOS>=3); INT21h AH=48h allocate; INT21h AH=4Ah
; ASMDOC resize twice
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: If end segment is already nonzero skips DOS allocation but resets table boundaries/states. On
; ASMDOC pre-DOS3 path PSP offset fallback is CS, not an independently fetched PSP. Allocation CF is not checked;
; ASMDOC two identical AH=4Ah calls are real.
_mmgr_alloc_resmem proc far
    arg_exclusive_upper_segment = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, 3000h
    int     R5HW_DOS_INT ; DOS - GET DOS VERSION                                          
    mov     bx, cs
    cmp     al, 3
    jl      short resmem_alloc_branch_path_01
    mov     ah, 62h
    int     R5HW_DOS_INT ; DOS - 3+ - GET PSP ADDRESS                                          
resmem_alloc_branch_path_01:
    mov     _pspofs, bx
    mov     _pspseg, ds
    cmp     _resmem_end_seg, 0
    jnz     short resmem_alloc_loop_head_01
    mov     bx, 64h ; 'd'
    mov     ah, 48h
    int     R5HW_DOS_INT ; DOS - 2+ - ALLOCATE MEMORY                                                               
    mov     si, _resptr1
    mov     [si+MEMCHUNK.resofs], ax
    mov     _resmem_base_seg, ax
    mov     es, ax
    mov     bx, [bp+arg_exclusive_upper_segment]
    sub     bx, [si+MEMCHUNK.resofs]
    mov     ah, 4Ah
    int     R5HW_DOS_INT
; DOS - 2+ - ADJUST MEMORY BLOCK SIZE (SETBLOCK)                                                               
    mov     ah, 4Ah
    int     R5HW_DOS_INT
; DOS - 2+ - ADJUST MEMORY BLOCK SIZE (SETBLOCK)                                                               
    mov     ax, _resmem_base_seg
    add     ax, bx
    mov     si, _resendptr2
    mov     [si+MEMCHUNK.resofs], ax
    mov     _resmem_end_seg, ax
; ASMDOC Shared initialized-arena join used by alloc_resmem and nopsub_3111D; restores SI/DI/BP saved by either
; ASMDOC entry.
resmem_alloc_loop_head_01:
    mov     si, _resendptr2
    mov     _resendptr1, si
    mov     si, _resptr1
    mov     _resptr2, si
resmem_alloc_loop_head_02:
    add     si, 12h
    cmp     si, _resendptr2
    jz      short resmem_alloc_branch_path_02
    mov     [si+MEMCHUNK.res_flags], 0
    jmp     short resmem_alloc_loop_head_02
resmem_alloc_branch_path_02:
    pop     di
    pop     si
    pop     bp
    retf
_mmgr_alloc_resmem endp
; -----------------------------------------------------------------------------
; Purpose: Allocate a memory block at the A0000h graphics-memory base.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs
; and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
;                                                       
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_alloc_a000 (_mmgr_alloc_a000): Initialize resource arena using exclusive upper segment
; ASMDOC A000h.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: none directly identified
; ASMDOC outputs: Same arena effects as mmgr_alloc_resmem(A000h); no defined scalar result
; ASMDOC preserves: SI; DI; BP; DS if DOS preserves it
; ASMDOC clobbers: AX; BX; ES; DOS-dependent effects/flags
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP;
; ASMDOC _resmem_end_seg
; ASMDOC globals_written: _pspofs; _pspseg; _resmem_base_seg; _resmem_end_seg; _resptr2; _resendptr1; interior
; ASMDOC MEMCHUNK state words
; ASMDOC services: INT21h AX=3000h DOS version; INT21h AH=62h PSP (DOS>=3); INT21h AH=48h allocate; INT21h AH=4Ah
; ASMDOC resize twice
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: A000h is an upper conventional-memory boundary. This does not allocate VGA pixels or return
; ASMDOC A000:0000.
_initmemory proc far

    mov     ax, R5HW_VIDEO_GRAPHICS_SEGMENT ;                                                      
    push    ax
    call    _mmgr_alloc_resmem
    add     sp, 2
    retf
_initmemory endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 310FE.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _resendptr2, _resmem_end_seg.
; Globals written: _resmem_end_seg.
;                                                       
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_310FE (_mmgr_init_video_reserve): Initialize arena below A000h, then subtract caller
; ASMDOC reserve paragraphs from upper sentinel segment and global end.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 u16 reserve paragraphs
; ASMDOC outputs: Upper end reduced modulo 65536; no defined scalar result
; ASMDOC preserves: SI; DI; BP; DS if DOS preserves it
; ASMDOC clobbers: AX; BX; ES; DOS-dependent effects/flags
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP;
; ASMDOC _resmem_end_seg
; ASMDOC globals_written: _pspofs; _pspseg; _resmem_base_seg; _resmem_end_seg; _resptr2; _resendptr1; interior
; ASMDOC MEMCHUNK state words; high sentinel segment
; ASMDOC services: INT21h AX=3000h DOS version; INT21h AH=62h PSP (DOS>=3); INT21h AH=48h allocate; INT21h AH=4Ah
; ASMDOC resize twice
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Notes: Reservation subtraction runs even if allocator skipped DOS allocation. No underflow guard.
_mmgr_init_video_reserve proc far
    arg_reserve_paragraphs = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, R5HW_VIDEO_GRAPHICS_SEGMENT ;                                                      
    push    ax
    call    _mmgr_alloc_resmem
    add     sp, 2
    mov     ax, [bp+arg_reserve_paragraphs]
    mov     bx, _resendptr2
    sub     [bx+MEMCHUNK.resofs], ax
    sub     _resmem_end_seg, ax
    pop     bp
    retf
_mmgr_init_video_reserve endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 3111D. Its internal flow uses noop_entry2_branch_path_01.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_0 (word ptr 6).
; Register ABI: touches AX, BX, SI, DI, BP, SP, CS, DS; balanced explicit saves/restores observed: none
; inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _resendptr2, _resmem_end_seg, _resptr1.
; Globals written: _pspofs, _pspseg, _resmem_base_seg, _resmem_end_seg.
;                                                               
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_3111D (_mmgr_alloc_dos_heap): Initialize arena using DOS paragraph count; retry once with
; ASMDOC BX returned by a failed allocation, then share descriptor reset epilogue.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 u16 requested paragraphs; DS=DGROUP
; ASMDOC outputs: Base/end segments and table initialized; no defined scalar result
; ASMDOC preserves: SI; DI; BP; DS if DOS preserves it
; ASMDOC clobbers: AX; BX; DOS-dependent effects/flags
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP;
; ASMDOC _resmem_end_seg
; ASMDOC globals_written: _pspofs; _pspseg; _resmem_base_seg; _resmem_end_seg; _resptr2; _resendptr1; interior
; ASMDOC MEMCHUNK state words
; ASMDOC services: INT21h AH=48h once, or twice if first allocation CF=1
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Notes: Shares resmem_alloc_loop_head_01 and return in mmgr_alloc_resmem. If retry also fails there is no
; ASMDOC further error check. Records PSP offset as CS directly.
_initmemsize proc far
    arg_requested_paragraphs = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     _pspofs, cs
    mov     _pspseg, ds
    cmp     _resmem_end_seg, 0
    jnz     short resmem_alloc_loop_head_01
    mov     bx, [bp+arg_requested_paragraphs]
    mov     ah, 48h
    int     R5HW_DOS_INT ; DOS - 2+ - ALLOCATE MEMORY                                                               
    jnb     short noop_entry2_branch_path_01
    mov     ah, 48h
    int     R5HW_DOS_INT ; DOS - 2+ - ALLOCATE MEMORY                                                               
noop_entry2_branch_path_01:
    mov     si, _resptr1
    mov     [si+MEMCHUNK.resofs], ax
    mov     _resmem_base_seg, ax
    add     ax, bx
    mov     si, _resendptr2
    mov     [si+MEMCHUNK.resofs], ax
    mov     _resmem_end_seg, ax
    jmp     resmem_alloc_loop_head_01
_initmemsize endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 31157.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX; balanced explicit saves/restores observed: none inferred; path-specific inputs
; and outputs follow the listed operands.
; Globals read: _resendptr1, _resptr2.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_31157 (_mmgr_get_free_paragraphs): Return the paragraph gap between low top and first
; ASMDOC high descriptor.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: DS=DGROUP
; ASMDOC outputs: AX=(resendptr1.segment - resptr2.segment - resptr2.size) modulo 65536; BX=last descriptor
; ASMDOC referenced
; ASMDOC preserves: CX; DX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; BX; arithmetic flags
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Evidence: lib/memmgr.ASM:191
_poolmemory proc far

    mov     bx, _resendptr1
    mov     ax, [bx+MEMCHUNK.resofs]
    mov     bx, _resptr2
    sub     ax, [bx+MEMCHUNK.resofs]
    sub     ax, [bx+MEMCHUNK.ressize]
    retf
_poolmemory endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 31169.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX; balanced explicit saves/restores observed: none inferred; path-specific inputs
; and outputs follow the listed operands.
; Globals read: _resptr1, _resptr2.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_31169 (_mmgr_get_low_used_paragraphs): Return total low-side paragraph span including
; ASMDOC holes.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: DS=DGROUP
; ASMDOC outputs: AX=(resptr2.segment + resptr2.size - resptr1.segment) modulo 65536; BX=last descriptor referenced
; ASMDOC preserves: CX; DX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; BX; arithmetic flags
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Evidence: lib/memmgr.ASM:207
_lockedmemory proc far

    mov     bx, _resptr2
    mov     ax, [bx+MEMCHUNK.resofs]
    add     ax, [bx+MEMCHUNK.ressize]
    mov     bx, _resptr1
    sub     ax, [bx+MEMCHUNK.resofs]
    retf
_lockedmemory endp
; -----------------------------------------------------------------------------
; Purpose: Return the offset difference between two memory locations.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX; balanced explicit saves/restores observed: none inferred; path-specific inputs
; and outputs follow the listed operands.
; Globals read: _resendptr2, _resptr2.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_get_ofs_diff (_mmgr_get_ofs_diff): Return paragraph gap between low top and absolute arena
; ASMDOC end; includes high-cache occupancy.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: DS=DGROUP
; ASMDOC outputs: AX=(resendptr2.segment - resptr2.segment - resptr2.size) modulo 65536; BX=last descriptor
; ASMDOC referenced
; ASMDOC preserves: CX; DX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; BX; arithmetic flags
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Evidence: lib/memmgr.ASM:223
_unlockedmemory proc far

    mov     bx, _resendptr2
    mov     ax, [bx+MEMCHUNK.resofs]
    mov     bx, _resptr2
    sub     ax, [bx+MEMCHUNK.resofs]
    sub     ax, [bx+MEMCHUNK.ressize]
    retf
_unlockedmemory endp
; -----------------------------------------------------------------------------
; Purpose: Copy a block of memory measured in DOS paragraphs.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8); arg_4 (word ptr 10).
; Register ABI: touches AX, BX, CX, SI, DI, BP, SP, DS, ES; balanced explicit saves/restores observed: DI, DS,
; SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_copy_paras (_mmgr_copy_paras): Copy exactly count*16 bytes forward for downward overlapping
; ASMDOC moves.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 u16 source segment (offset zero); BP+8 u16 destination segment (offset zero); BP+10 u16
; ASMDOC paragraph count
; ASMDOC outputs: Destination paragraph bytes copied in ascending word order; DF=0 on return; no scalar result
; ASMDOC preserves: DX; SI; DI; BP; DS; IF
; ASMDOC clobbers: AX; BX; CX; ES; arithmetic flags; DF cleared
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Splits at 1000h paragraphs (64 KiB) to preserve 16-bit SI/DI traversal. Segment
; ASMDOC additions/subtractions wrap as words. Sets CLD even for count zero.
_posblockmove proc far
    arg_source_segment = word ptr 6
    arg_destination_segment = word ptr 8
    arg_paragraph_count = word ptr 10

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    cld
    mov     ds, [bp+arg_source_segment]
    mov     es, [bp+arg_destination_segment]
    mov     bx, [bp+arg_paragraph_count]
; ASMDOC 1000h paragraphs = 8000h words = one full 64 KiB MOVSW block; segment steps prevent SI/DI wrap from losing
; ASMDOC position.
copy_fwd_loop_head_01:
    or      bx, bx
    jz      short copy_fwd_branch_path_02
    mov     cx, 8000h
    mov     ax, bx
    sub     bx, 1000h
    jnb     short copy_fwd_branch_path_01
    add     bx, 1000h
    shl     bx, 1
    shl     bx, 1
    shl     bx, 1
    mov     cx, bx
    xor     bx, bx
copy_fwd_branch_path_01:
    xor     si, si
    xor     di, di
    rep movsw
    mov     ax, ds
    add     ax, 1000h
    mov     ds, ax
    mov     ax, es
    add     ax, 1000h
    mov     es, ax
    jmp     short copy_fwd_loop_head_01
copy_fwd_branch_path_02:
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
_posblockmove endp
; -----------------------------------------------------------------------------
; Purpose: Copy a paragraph block in reverse order to handle overlap.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8); arg_4 (word ptr 10).
; Register ABI: touches AX, BX, CX, SI, DI, BP, SP, DS, ES; balanced explicit saves/restores observed: DI, DS,
; SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _copy_paras_reverse (_copy_paras_reverse): Copy exactly count*16 bytes backward for upward
; ASMDOC overlapping moves.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 u16 source segment (offset zero); BP+8 u16 destination segment (offset zero); BP+10 u16
; ASMDOC paragraph count
; ASMDOC outputs: Destination paragraph bytes copied in descending word order; DF=0 on return; no scalar result
; ASMDOC preserves: DX; SI; DI; BP; DS; IF
; ASMDOC clobbers: AX; BX; CX; ES; arithmetic flags; DF cleared
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Splits at 1000h paragraphs (64 KiB) to preserve 16-bit SI/DI traversal. Segment
; ASMDOC additions/subtractions wrap as words. Sets STD then explicitly CLD even for count zero.
_negblockmove proc far
    arg_source_segment = word ptr 6
    arg_destination_segment = word ptr 8
    arg_paragraph_count = word ptr 10

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    std
    mov     bx, [bp+arg_paragraph_count]
    mov     ax, [bp+arg_source_segment]
    add     ax, bx
    mov     ds, ax
    mov     ax, [bp+arg_destination_segment]
    add     ax, bx
    mov     es, ax
; ASMDOC Begin from exclusive source/destination ends, step down by block paragraphs, then copy words backward.
copy_rev_loop_head_01:
    or      bx, bx
    jz      short copy_rev_branch_path_02
    mov     cx, 1000h
    sub     bx, 1000h
    jnb     short copy_rev_branch_path_01
    add     bx, 1000h
    mov     cx, bx
    xor     bx, bx
copy_rev_branch_path_01:
    mov     ax, ds
    sub     ax, cx
    mov     ds, ax
    mov     ax, es
    sub     ax, cx
    mov     es, ax
    shl     cx, 1
    shl     cx, 1
    shl     cx, 1
    mov     ax, cx
    shl     ax, 1
    dec     ax
    dec     ax
    mov     si, ax
    mov     di, ax
    rep movsw
    jmp     short copy_rev_loop_head_01
copy_rev_branch_path_02:
    cld
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
_negblockmove endp
; -----------------------------------------------------------------------------
; Purpose: Extract a resource name from its path.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_filename (word ptr 6).
; Register ABI: touches AX, BX, SI, BP, SP; balanced explicit saves/restores observed: SI; path-specific inputs
; and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_path_to_name (_mmgr_path_to_name): Return near address following the last colon or
; ASMDOC backslash in a NUL-terminated path.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 near DS offset of readable NUL-terminated path
; ASMDOC outputs: AX=near basename offset (may address terminal NUL); BX=same offset; DF=0
; ASMDOC preserves: CX; DX; SI; DI; BP; DS; ES; IF
; ASMDOC clobbers: AX; BX; arithmetic flags; DF cleared
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Does not strip extension, fold case, treat forward slash as delimiter, copy a name, or validate
; ASMDOC pointer. Scans with 16-bit SI wrap.
_mmgr_path_to_name proc far
    arg_filename = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    mov     si, [bp+arg_filename]
    mov     bx, si
    cld
path_name_loop_head_01:
    lodsb
    or      al, al
    jz      short path_name_branch_path_02
    cmp     al, 3Ah ; ':'
    jz      short path_name_branch_path_01
    cmp     al, 5Ch ; '\'
    jnz     short path_name_loop_head_01
path_name_branch_path_01:
    mov     bx, si
    jmp     short path_name_loop_head_01
path_name_branch_path_02:
    mov     ax, bx
    pop     si
    pop     bp
    retf
_mmgr_path_to_name endp
; -----------------------------------------------------------------------------
; Purpose: Allocate the requested number of memory pages.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _resendptr1, _resendptr2, _resmaxsize, _resptr2.
; Globals written: _resendptr1, _resmaxsize, _resptr2.
; Global addresses taken: _aReservememoryO, _aReservememoryOutOfMemory.
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_alloc_pages (_mmgr_alloc_pages): Reserve low-side descriptor and paragraph span; invalidate
; ASMDOC overlapping high cache and report exhaustion through fatal_error.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 near DS pathname pointer, readable 12 bytes starting at basename; BP+8 u16 paragraph count;
; ASMDOC DS=DGROUP
; ASMDOC outputs: DX:AX=new segment:0000; New descriptor has 12 copied name bytes, requested size, state=2;
; ASMDOC resmaxsize tracks largest low ending segment
; ASMDOC preserves: SI; DI; BP; DS (including copy-helper restoration); IF
; ASMDOC clobbers: AX; BX; CX; DX; arithmetic flags; DF cleared
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP; _resmaxsize
; ASMDOC globals_written: _resptr2; _resendptr1; _resmaxsize; new low descriptor; overlapped high descriptor state
; ASMDOC words
; ASMDOC services: _mmgr_path_to_name; _fatal_error (expected not to return)
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Copies all 12 bytes without stopping at NUL. Returns normalized far pointer with offset zero.
; ASMDOC Paragraph arithmetic wraps in 16 bits; no explicit wrap guard. Fatal calls assume nonreturn; returning
; ASMDOC would enter shared/cross-procedure code with arguments still stacked.
_reservememory proc far
    arg_pathname = word ptr 6
    arg_paragraph_count = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     di, _resptr2
    mov     si, _resendptr1
    mov     dx, [di+MEMCHUNK.resofs]
    add     dx, [di+MEMCHUNK.ressize]
    add     di, 12h
    cmp     si, di
    jbe     short pages_alloc_branch_path_02
pages_alloc_loop_head_01:
    mov     _resptr2, di
    push    [bp+arg_pathname]
    call    _mmgr_path_to_name
    add     sp, 2
    mov     si, ax
    xor     bx, bx
    mov     cx, 0Ch
; ASMDOC Always copy 12 basename bytes, including bytes beyond a NUL; no string padding or length check here.
pages_alloc_loop_head_02:
    mov     al, [bx+si+MEMCHUNK.resname]
    mov     [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    pages_alloc_loop_head_02
    mov     si, _resendptr1
    mov     ax, [bp+arg_paragraph_count]
    mov     [di+MEMCHUNK.resofs], dx
    mov     [di+MEMCHUNK.ressize], ax
    mov     [di+MEMCHUNK.res_flags], 2
    add     ax, dx
    cmp     ax, _resmaxsize
    jb      short pages_alloc_branch_path_01
    mov     _resmaxsize, ax
pages_alloc_branch_path_01:
    cmp     ax, [si+MEMCHUNK.resofs]
    ja      short pages_alloc_branch_path_03
pages_alloc_loop_head_03:
    xor     ax, ax
    pop     di
    pop     si
    pop     bp
    retf
pages_alloc_branch_path_02:
    cmp     si, _resendptr2
    jz      short pages_alloc_error_path_01
    add     si, 12h
    mov     _resendptr1, si
    jmp     short pages_alloc_loop_head_01
pages_alloc_error_path_01:
    push    [bp+arg_pathname]
    mov     ax, offset DGROUP:_noblkerr; "reservememory - OUT OF MEMORY SLOTS RES"...
    push    ax
    call    far ptr _abortmessage
pages_alloc_branch_path_03:
    mov     si, _resendptr1
    mov     di, _resptr2
    mov     ax, [di+MEMCHUNK.resofs]
    add     ax, [di+MEMCHUNK.ressize]
; ASMDOC Evict high cached descriptors whose starting segments overlap the new low allocation end.
pages_alloc_loop_head_04:
    cmp     ax, [si+MEMCHUNK.resofs]
    jbe     short pages_alloc_loop_head_03
    cmp     si, _resendptr2
    jz      short pages_alloc_branch_path_04
    mov     [si+MEMCHUNK.res_flags], 0
    add     si, 12h
    mov     _resendptr1, si
    jmp     short pages_alloc_loop_head_04
pages_alloc_branch_path_04:
    mov     bx, _resmaxsize
    push    bx
    push    [di+MEMCHUNK.ressize]
    push    [bp+arg_pathname]
    mov     ax, offset DGROUP:_nomemerr; "reservememory - OUT OF MEMORY RESERVING"...
    push    ax
    call    far ptr _abortmessage
_reservememory endp
; -----------------------------------------------------------------------------
; Purpose: Find a free memory-manager entry for the requested block.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, CX, DX, SI, DI, SP; balanced explicit saves/restores observed: DI, DX, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _resendptr1, _resendptr2.
; Globals written: _resendptr1.
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_find_free (_mmgr_find_free): Compact state-bit0 high-side descriptors upward over non-bit0
; ASMDOC descriptors, preserving cached bytes with reverse paragraph copies.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: DS=DGROUP; valid high table boundaries
; ASMDOC outputs: resendptr1 moved to first resulting high descriptor; May relocate high cached data and descriptor
; ASMDOC segments; no scalar result
; ASMDOC preserves: DX; SI; DI; BP; DS; IF
; ASMDOC clobbers: AX; BX; CX; ES if copy path; arithmetic flags; DF cleared on copy path
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP
; ASMDOC globals_written: _resendptr1; high MEMCHUNK descriptors; high resource byte spans
; ASMDOC services: _copy_paras_reverse
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: No free-block handle or descriptor is returned. Selects keepers by TEST state,1, rather than
; ASMDOC state!=0. Accumulated DX gap is restored to caller at exit.
_mmgr_find_free proc far
    push    si
    push    di
    push    dx
    mov     si, _resendptr2
    mov     di, si
    xor     dx, dx
; ASMDOC State bit 0 selects retained high chunks. Non-bit0 sizes accumulate as a gap; later keepers move upward
; ASMDOC with reverse copy.
find_free_loop_head_01:
    test    [si+MEMCHUNK.res_flags], 01h
    jnz     short find_free_branch_path_01
    add     dx, [si+MEMCHUNK.ressize]
    jmp     short find_free_join_01
    nop ; retained code pad at an even offset; EVEN would emit no byte
find_free_branch_path_01:
    or      dx, dx
    jnz     short find_free_branch_path_02
find_free_loop_head_02:
    sub     di, 12h
find_free_join_01:
    sub     si, 12h
    cmp     si, _resendptr1
    jnb     short find_free_loop_head_01
    add     di, 12h
    mov     _resendptr1, di
    pop     dx
    pop     di
    pop     si
    retf
find_free_branch_path_02:
    mov     bx, [si+MEMCHUNK.ressize]
    push    bx
    add     di, 12h
    mov     ax, [di+MEMCHUNK.resofs]
    sub     ax, bx
    push    ax
    push    [si+MEMCHUNK.resofs]
    sub     di, 12h
    mov     [di+MEMCHUNK.ressize], bx
    mov     [di+MEMCHUNK.resofs], ax
    mov     cx, [si+MEMCHUNK.res_flags]
    mov     [si+MEMCHUNK.res_flags], 0
    mov     [di+MEMCHUNK.res_flags], cx
    xor     bx, bx
    mov     cx, 0Ch
find_free_loop_head_03:
    mov     al, [bx+si+MEMCHUNK.resname]
    mov     [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    find_free_loop_head_03
    call    _negblockmove
    add     sp, 6
    jmp     short find_free_loop_head_02
_mmgr_find_free endp
; -----------------------------------------------------------------------------
; Purpose: Find a memory-manager chunk by its name.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _resendptr1, _resendptr2, _resptr2.
; Globals written: _resendptr1, _resptr2.
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_get_chunk_by_name (_mmgr_get_chunk_by_name): Find high cached basename, move it to low live
; ASMDOC side, discard overlapping cache, then compact remaining high records.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 near DS NUL-terminated pathname; DS=DGROUP
; ASMDOC outputs: DX:AX=low destination segment:0000 on hit; 0:0 on miss; Cached source state cleared, destination
; ASMDOC state=2
; ASMDOC preserves: SI; DI; BP; DS (including copy-helper restoration); IF
; ASMDOC clobbers: AX; BX; CX; DX; ES when paragraph-copy path executes; arithmetic flags; DF cleared when
; ASMDOC copy/path helper executes
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP
; ASMDOC globals_written: _resptr2; _resendptr1; low/high MEMCHUNK descriptors; relocated resource bytes
; ASMDOC services: _mmgr_path_to_name; _mmgr_copy_paras; _mmgr_find_free (transitively _copy_paras_reverse)
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Compares case-sensitively up to 12 bytes. A shorter query matches stored dot or NUL; zero-state
; ASMDOC descriptor terminates search immediately. Not a read-only lookup and does not return the old high address.
_checkmodule proc far
    arg_pathname = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     si, _resendptr1
    push    [bp+arg_pathname]
    call    _mmgr_path_to_name
    add     sp, 2
    mov     di, ax
find_chunk_loop_head_01:
    xor     bx, bx
    cmp     [si+MEMCHUNK.res_flags], 0
    jz      short find_chunk_branch_path_03
find_chunk_loop_head_02:
    mov     al, [bx+di]
    or      al, al
    jz      short find_chunk_branch_path_01
    cmp     al, [bx+si+MEMCHUNK.resname]
    jnz     short find_chunk_branch_path_02
    inc     bx
    cmp     bx, 0Ch
    jl      short find_chunk_loop_head_02
    jmp     short find_chunk_branch_path_04
    nop ; retained code pad at an even offset; EVEN would emit no byte
; ASMDOC An exhausted query can match a stored extension separator; comparison is otherwise case-sensitive.
find_chunk_branch_path_01:
    cmp     byte ptr [bx+si+MEMCHUNK.resname], 2Eh ; '.'
    jz      short find_chunk_branch_path_04
    cmp     byte ptr [bx+si+MEMCHUNK.resname], 0
    jz      short find_chunk_branch_path_04
find_chunk_branch_path_02:
    add     si, 12h
    cmp     si, _resendptr2
    jb      short find_chunk_loop_head_01
find_chunk_branch_path_03:
    xor     dx, dx
find_chunk_loop_head_03:
    xor     ax, ax
    pop     di
    pop     si
    pop     bp
    retf
; ASMDOC A cache hit becomes a new low live allocation; subsequent high compaction can relocate remaining cache
; ASMDOC records.
find_chunk_branch_path_04:
    mov     di, _resptr2
    mov     dx, [di+MEMCHUNK.resofs]
    add     dx, [di+MEMCHUNK.ressize]
    add     di, 12h
    mov     _resptr2, di
    mov     ax, [si+MEMCHUNK.ressize]
    push    ax
    push    dx
    push    [si+MEMCHUNK.resofs]
    mov     [si+MEMCHUNK.res_flags], 0
    mov     [di+MEMCHUNK.resofs], dx
    mov     [di+MEMCHUNK.ressize], ax
    mov     [di+MEMCHUNK.res_flags], 2
    xor     bx, bx
    mov     cx, 0Ch
find_chunk_loop_head_04:
    mov     al, [bx+si+MEMCHUNK.resname]
    mov     [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    find_chunk_loop_head_04
    cmp     di, _resendptr1
    jnz     short find_chunk_branch_path_05
    add     _resendptr1, 12h
find_chunk_branch_path_05:
    call    _posblockmove
    add     sp, 6
    mov     si, _resendptr1
    mov     di, _resptr2
    mov     ax, [di+MEMCHUNK.resofs]
    add     ax, [di+MEMCHUNK.ressize]
find_chunk_loop_head_05:
    cmp     ax, [si+MEMCHUNK.resofs]
    jbe     short find_chunk_branch_path_06
    mov     [si+MEMCHUNK.res_flags], 0
    add     si, 12h
    mov     _resendptr1, si
    jmp     short find_chunk_loop_head_05
find_chunk_branch_path_06:
    call    _mmgr_find_free
    mov     dx, [di+MEMCHUNK.resofs]
    jmp     short find_chunk_loop_head_03
_checkmodule endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 31429. Its internal flow uses noop_entry5_loop_head_01,
; noop_entry5_loop_head_02.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _resendptr1, _resendptr2.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_31429 (_mmgr_has_high_resource): Test high-side name membership using the same basename
; ASMDOC match and early zero-state stop, without relocating data.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 near DS NUL-terminated pathname; DS=DGROUP
; ASMDOC outputs: AX=1 on match, 0 on miss; DX=0 on miss; otherwise incoming DX unchanged
; ASMDOC preserves: CX; SI; DI; BP; DS; ES; IF
; ASMDOC clobbers: AX; BX; DX on miss; arithmetic flags; DF cleared
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP
; ASMDOC globals_written: none directly identified
; ASMDOC services: _mmgr_path_to_name
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Notes: Case-sensitive 12-byte maximum; shorter basename may match stored extension. Reads descriptor state
; ASMDOC at +10h; no cache population.
_ifmodule proc far
    arg_pathname = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     si, _resendptr1
    push    [bp+arg_pathname]
    call    _mmgr_path_to_name
    add     sp, 2
    mov     di, ax
noop_entry5_loop_head_01:
    xor     bx, bx
    cmp     word ptr [si+10h], 0
    jz      short noop_entry5_branch_path_03
noop_entry5_loop_head_02:
    mov     al, [bx+di]
    or      al, al
    jz      short noop_entry5_branch_path_01
    cmp     al, [bx+si]
    jnz     short noop_entry5_branch_path_02
    inc     bx
    cmp     bx, 0Ch
    jl      short noop_entry5_loop_head_02
    jmp     short noop_entry5_branch_path_04
    even ; word-align the following entry
noop_entry5_branch_path_01:
    cmp     byte ptr [bx+si], 2Eh ; '.'
    jz      short noop_entry5_branch_path_04
    cmp     byte ptr [bx+si], 0
    jz      short noop_entry5_branch_path_04
noop_entry5_branch_path_02:
    add     si, 12h
    cmp     si, _resendptr2
    jb      short noop_entry5_loop_head_01
noop_entry5_branch_path_03:
    xor     dx, dx
    xor     ax, ax
    pop     di
    pop     si
    pop     bp
    retf
noop_entry5_branch_path_04:
    mov     ax, 1
    pop     di
    pop     si
    pop     bp
    retf
_ifmodule endp
; -----------------------------------------------------------------------------
; Purpose: Release the selected memory-manager allocation.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _resendptr1, _resptr1, _resptr2.
; Globals written: _resendptr1, _resptr2.
; Global addresses taken: _aMemoryManagerB.
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_free (_mmgr_free): Remove low chunk by segment, optionally reverse-copy it to high cache
; ASMDOC when room permits, then retreat low top across holes.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 far pointer offset word (retained); BP+8 far pointer segment selecting low descriptor; offset
; ASMDOC is not used for lookup; DS=DGROUP
; ASMDOC outputs: AX=original offset; DX=cached high segment if retained, zero if discarded; Caller argument slot
; ASMDOC BP+8 is overwritten with zero or cache segment
; ASMDOC preserves: SI; DI; BP; DS (including copy-helper restoration); IF
; ASMDOC clobbers: AX; BX; CX; DX; ES when paragraph-copy path executes; arithmetic flags; DF cleared when
; ASMDOC copy/path helper executes
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP
; ASMDOC globals_written: _resptr2; _resendptr1; low/high descriptors; high cached bytes; stack argument BP+8
; ASMDOC services: _copy_paras_reverse; _fatal_error on unknown segment (nonreturn expected)
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: This is cache transfer/discard, not DOS AH=49h free. Segment search excludes low sentinel. Internal
; ASMDOC free_chunk_loop_head_02 is shared fatal target of multiple routines.
_freememory proc far
    arg_unused_offset = word ptr 6
    arg_chunk_segment = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_chunk_segment]
    mov     si, _resptr2
free_chunk_loop_head_01:
    cmp     si, _resptr1
    jz      short free_chunk_loop_head_02
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short free_chunk_branch_path_01
    sub     si, 12h
    jmp     short free_chunk_loop_head_01
; ASMDOC Shared missing-segment fatal path. Several procedures jump here with different frames; fatal_error must
; ASMDOC not return.
free_chunk_loop_head_02:
    push    [bp+arg_chunk_segment]
    mov     ax, offset DGROUP:_blknotfound; "memory manager - BLOCK NOT FOUND at SEG"...
    push    ax
    call    far ptr _abortmessage
free_chunk_branch_path_01:
    mov     [bp+arg_chunk_segment], 0
    mov     [si+MEMCHUNK.res_flags], 0
    cmp     si, _resptr2
    jz      short free_chunk_branch_path_02
    mov     bx, _resptr2
    mov     di, _resendptr1
    cmp     si, di
    jz      short free_chunk_branch_path_03
    mov     ax, [di+MEMCHUNK.resofs]
    sub     ax, [bx+MEMCHUNK.resofs]
    sub     ax, [bx+MEMCHUNK.ressize]
    cmp     ax, [si+MEMCHUNK.ressize]
    jb      short free_chunk_branch_path_03
; ASMDOC Cache the released bytes below the high boundary when the gap fits; return cache segment through mutated
; ASMDOC argument word.
free_chunk_branch_path_02:
    mov     bx, [si+MEMCHUNK.ressize]
    push    bx
    mov     di, _resendptr1
    mov     ax, [di+MEMCHUNK.resofs]
    sub     ax, bx
    push    ax
    mov     [bp+arg_chunk_segment], ax
    push    [si+MEMCHUNK.resofs]
    sub     di, 12h
    mov     _resendptr1, di
    mov     [di+MEMCHUNK.resofs], ax
    mov     [di+MEMCHUNK.ressize], bx
    mov     [di+MEMCHUNK.res_flags], 1
    mov     cx, 0Ch
    xor     bx, bx
free_chunk_loop_head_03:
    mov     al, [bx+si+MEMCHUNK.resname]
    mov     [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    free_chunk_loop_head_03
    call    _negblockmove
    add     sp, 6
free_chunk_branch_path_03:
    cmp     si, _resptr2
    jnz     short free_chunk_branch_path_04
free_chunk_loop_head_04:
    sub     si, 12h
    cmp     [si+MEMCHUNK.res_flags], 0
    jz      short free_chunk_loop_head_04
    mov     _resptr2, si
free_chunk_branch_path_04:
    mov     ax, [bp+arg_unused_offset]
    mov     dx, [bp+arg_chunk_segment]
    pop     di
    pop     si
    pop     bp
    retf
_freememory endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 31525. Its internal flow uses noop_entry6_loop_head_01,
; noop_entry6_branch_path_01.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _resendptr1, _resendptr2, _resptr1, _resptr2.
; Globals written: _resendptr1, _resptr2.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_31525 (_mmgr_move_chunk_to_high): Remove low chunk and conditionally repack high region
; ASMDOC before caching the removed bytes at the absolute arena end.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 far pointer offset word (retained); BP+8 far pointer segment used for lookup; DS=DGROUP
; ASMDOC outputs: AX=original offset; DX=0 (BP+8 cleared even when a high copy is made); High region
; ASMDOC descriptors/data may be shifted downward; low top may retreat
; ASMDOC preserves: SI; DI; BP; DS (including copy-helper restoration); IF
; ASMDOC clobbers: AX; BX; CX; DX; ES when paragraph-copy path executes; arithmetic flags; DF cleared when
; ASMDOC copy/path helper executes
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP
; ASMDOC globals_written: _resptr2; _resendptr1; low/high descriptors; relocated resource bytes; stack argument
; ASMDOC BP+8
; ASMDOC services: _mmgr_copy_paras; _copy_paras_reverse; _fatal_error shared missing-segment path
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Notes: Has six-byte local frame, unlike free. Its missing-segment jump enters free fatal path; if
; ASMDOC fatal_error returned, incompatible locals/stack would make subsequent flow invalid. No cache address is
; ASMDOC written back on the copying path.
_freetotop proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
    arg_unused_offset = word ptr 6
    arg_chunk_segment = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 6
    push    si
    push    di
    mov     ax, [bp+arg_chunk_segment]
    mov     si, _resptr2
noop_entry6_loop_head_01:
    cmp     si, _resptr1
    jz      short noop_entry6_branch_path_01
    cmp     ax, [si+0Eh]
    jz      short noop_entry6_branch_path_02
    sub     si, 12h
    jmp     short noop_entry6_loop_head_01
noop_entry6_branch_path_01:
    jmp     free_chunk_loop_head_02
noop_entry6_branch_path_02:
    mov     [bp+arg_chunk_segment], 0
    mov     word ptr [si+10h], 0
    cmp     si, _resptr2
    jz      short noop_entry6_branch_path_03
    mov     bx, _resptr2
    mov     di, _resendptr2
    mov     ax, [di+0Eh]
    sub     ax, [bx+0Eh]
    sub     ax, [bx+0Ch]
    cmp     ax, [si+0Ch]
    ja      short noop_entry6_branch_path_03
    jmp     noop_entry6_join_01
noop_entry6_branch_path_03:
    mov     di, _resptr2
    mov     ax, [di+0Eh]
    add     ax, [di+0Ch]
    mov     [bp+var_2], ax
    mov     [bp+var_4], 0
    mov     di, _resendptr1
; ASMDOC Dead alternate release repacks qualifying high chunks downward, then inserts the removed chunk at absolute
; ASMDOC high end.
noop_entry6_loop_head_02:
    cmp     di, _resendptr2
    jz      short noop_entry6_branch_path_06
    mov     ax, [di+0Eh]
    sub     ax, [si+0Ch]
    cmp     [bp+var_2], ax
    ja      short noop_entry6_branch_path_05
    mov     bx, di
    sub     bx, 12h
    cmp     bx, _resptr2
    jz      short noop_entry6_branch_path_05
    cmp     [bp+var_4], 0
    jnz     short noop_entry6_branch_path_04
    mov     _resendptr1, bx
    mov     [bp+var_4], 1
noop_entry6_branch_path_04:
    mov     ax, [di+10h]
    mov     [bx+10h], ax
    mov     ax, [di+0Ch]
    mov     [bx+0Ch], ax
    push    ax
    mov     ax, [di+0Eh]
    sub     ax, [si+0Ch]
    mov     [bx+0Eh], ax
    push    ax
    push    word ptr [di+0Eh]
    mov     cx, 0Ch
noop_entry6_loop_head_03:
    mov     al, [di]
    mov     [bx], al
    inc     bx
    inc     di
    loop    noop_entry6_loop_head_03
    sub     di, 0Ch
    sub     bx, 0Ch
    call    _posblockmove
    add     sp, 6
noop_entry6_branch_path_05:
    add     di, 12h
    jmp     short noop_entry6_loop_head_02
noop_entry6_branch_path_06:
    mov     ax, [di+0Eh]
    sub     di, 12h
    cmp     [bp+var_4], 0
    jnz     short noop_entry6_branch_path_07
    mov     _resendptr1, di
noop_entry6_branch_path_07:
    mov     bx, [si+0Ch]
    mov     [di+0Ch], bx
    push    bx
    sub     ax, bx
    mov     [di+0Eh], ax
    push    ax
    push    word ptr [si+0Eh]
    mov     word ptr [di+10h], 1
    mov     cx, 0Ch
    xor     bx, bx
noop_entry6_loop_head_04:
    mov     al, [bx+si]
    mov     [bx+di], al
    inc     bx
    loop    noop_entry6_loop_head_04
    call    _negblockmove
    add     sp, 6
noop_entry6_join_01:
    cmp     si, _resptr2
    jnz     short noop_entry6_branch_path_08
noop_entry6_loop_head_05:
    sub     si, 12h
    cmp     word ptr [si+10h], 0
    jz      short noop_entry6_loop_head_05
    mov     _resptr2, si
noop_entry6_branch_path_08:
    mov     ax, [bp+arg_unused_offset]
    mov     dx, [bp+arg_chunk_segment]
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
_freetotop endp
; -----------------------------------------------------------------------------
; Purpose: Release memory-manager state associated with the selected block.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_2 (word ptr 8).
; Register ABI: touches AX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _resptr1, _resptr2.
; Globals written: _resptr2.
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_release (_mmgr_release): Clear a low descriptor without copying it to high cache; retreat
; ASMDOC low top across zero-state records if releasing top.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 far pointer offset word ignored; BP+8 far pointer segment used for lookup; DS=DGROUP
; ASMDOC outputs: Descriptor state=0; no defined scalar result
; ASMDOC preserves: BX; CX; DX; SI; DI; BP; DS; ES; DF; IF on successful path
; ASMDOC clobbers: AX; arithmetic flags
; ASMDOC globals_read: _resptr1; _resptr2; low descriptors
; ASMDOC globals_written: _resptr2; selected descriptor state
; ASMDOC services: _fatal_error shared missing-segment path
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Does not clear bytes or issue DOS free. Unknown segment enters free_chunk_loop_head_02; failure may
; ASMDOC have transitive effects from fatal_error.
_purgememory proc far
    arg_chunk_segment = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_chunk_segment]
    mov     si, _resptr2
release_chunk_loop_head_01:
    cmp     si, _resptr1
    jz      short release_chunk_branch_path_01
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short release_chunk_branch_path_02
    sub     si, 12h
    jmp     short release_chunk_loop_head_01
release_chunk_branch_path_01:
    jmp     free_chunk_loop_head_02
release_chunk_branch_path_02:
    mov     [si+MEMCHUNK.res_flags], 0
    cmp     si, _resptr2
    jnz     short release_chunk_branch_path_03
release_chunk_loop_head_02:
    sub     si, 12h
    cmp     [si+MEMCHUNK.res_flags], 0
    jz      short release_chunk_loop_head_02
    mov     _resptr2, si
release_chunk_branch_path_03:
    pop     di
    pop     si
    pop     bp
    retf
_purgememory endp
; -----------------------------------------------------------------------------
; Purpose: Return the size of the requested memory-manager chunk.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_2 (word ptr 8).
; Register ABI: touches AX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _resptr1, _resptr2.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_get_chunk_size (_mmgr_get_chunk_size): Find low descriptor by segment and return its size
; ASMDOC in paragraphs.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 far pointer offset word ignored; BP+8 far pointer segment used for lookup; DS=DGROUP
; ASMDOC outputs: AX=u16 descriptor size in paragraphs
; ASMDOC preserves: BX; CX; DX; SI; DI; BP; DS; ES; DF; IF on successful path
; ASMDOC clobbers: AX; arithmetic flags
; ASMDOC globals_read: _resptr1; _resptr2; low descriptors
; ASMDOC globals_written: none directly identified
; ASMDOC services: _fatal_error shared missing-segment path
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: No high-cache lookup; missing segment is fatal, not zero size.
_modulesize proc far
    arg_chunk_segment = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_chunk_segment]
    mov     si, _resptr2
chunk_size_loop_head_01:
    cmp     si, _resptr1
    jz      short chunk_size_branch_path_01
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short chunk_size_branch_path_02
    sub     si, 12h
    jmp     short chunk_size_loop_head_01
chunk_size_branch_path_01:
    jmp     free_chunk_loop_head_02
chunk_size_branch_path_02:
    mov     ax, [si+MEMCHUNK.ressize]
    pop     di
    pop     si
    pop     bp
    retf
_modulesize endp
; -----------------------------------------------------------------------------
; Purpose: Resize an existing memory-manager allocation.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_2 (word ptr 8); arg_4
; (word ptr 10).
; Register ABI: touches AX, BX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _resendptr1, _resendptr2, _resmaxsize, _resptr1, _resptr2.
; Globals written: _resendptr1, _resmaxsize.
; Global addresses taken: _aResizememoryCa, _aResizememoryNo.
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_resize_memory (_mmgr_resize_memory): Shrink selected low descriptor or grow only the
; ASMDOC current low top, invalidating high cache that overlaps new end.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 far pointer offset word ignored; BP+8 far pointer segment used for lookup; BP+10 u16 new size
; ASMDOC paragraphs; DS=DGROUP
; ASMDOC outputs: Shrink/equal path AX=new size; grow success AX=0 (no uniform scalar API result); Descriptor size
; ASMDOC updated; possible high invalidation; resmaxsize updated on growth
; ASMDOC preserves: CX; DX; SI; DI; BP; DS; ES; DF; IF on success
; ASMDOC clobbers: AX; BX on fatal-argument path; arithmetic flags
; ASMDOC globals_read: _resptr1; _resptr2; _resendptr1; _resendptr2; MEMCHUNK descriptors in DGROUP; _resmaxsize
; ASMDOC globals_written: selected descriptor size; _resendptr1; _resmaxsize; overlapped high descriptor states
; ASMDOC services: _fatal_error for missing segment, growing non-top block, or insufficient memory
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Size is written before overlap/exhaustion detection. No DOS resize here. Error callees are assumed
; ASMDOC nonreturn; memory and stack state already changed.
_resizememory proc far
    arg_chunk_segment = word ptr 8
    arg_new_paragraphs = word ptr 10

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_chunk_segment]
    mov     si, _resptr2
resize_mem_loop_head_01:
    cmp     si, _resptr1
    jz      short resize_mem_branch_path_01
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short resize_mem_branch_path_02
    sub     si, 12h
    jmp     short resize_mem_loop_head_01
resize_mem_branch_path_01:
    jmp     free_chunk_loop_head_02
resize_mem_branch_path_02:
    mov     ax, [bp+arg_new_paragraphs]
    cmp     ax, [si+MEMCHUNK.ressize]
    ja      short resize_mem_branch_path_03
    mov     [si+MEMCHUNK.ressize], ax
    pop     di
    pop     si
    pop     bp
    retf
; ASMDOC Only low top may grow. New size is stored before high-cache eviction and exhaustion checks.
resize_mem_branch_path_03:
    cmp     si, _resptr2
    jnz     short resize_mem_error_path_01
    mov     [si+MEMCHUNK.ressize], ax
    mov     di, _resendptr1
    add     ax, [si+MEMCHUNK.resofs]
    cmp     ax, _resmaxsize
    jb      short resize_mem_branch_path_04
    mov     _resmaxsize, ax
resize_mem_branch_path_04:
    cmp     ax, [di+MEMCHUNK.resofs]
    ja      short resize_mem_branch_path_05
resize_mem_loop_head_02:
    xor     ax, ax
    pop     di
    pop     si
    pop     bp
    retf
resize_mem_error_path_01:
    mov     ax, offset DGROUP:_expanderr; "resizememory - CANNOT EXPAND BLOCK NOT "...
    push    ax
    call    far ptr _abortmessage
resize_mem_branch_path_05:
    mov     si, _resendptr1
    mov     di, _resptr2
    mov     ax, [di+MEMCHUNK.resofs]
    add     ax, [di+MEMCHUNK.ressize]
resize_mem_loop_head_03:
    cmp     ax, [si+MEMCHUNK.resofs]
    jbe     short resize_mem_loop_head_02
    cmp     si, _resendptr2
    jz      short resize_mem_error_path_02
    mov     [si+MEMCHUNK.res_flags], 0
    add     si, 12h
    mov     _resendptr1, si
    jmp     short resize_mem_loop_head_03
resize_mem_error_path_02:
    mov     bx, _resmaxsize
    push    bx
    mov     ax, offset DGROUP:_exmemerr; "resizememory - NO MEMORY LEFT TO EXPAND"...
    push    ax
    call    far ptr _abortmessage
_resizememory endp
; -----------------------------------------------------------------------------
; Purpose: Perform the memory-manager operation identified by mmgr op unk.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_2 (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _resptr1, _resptr2.
; Globals written: _resptr2.
; -----------------------------------------------------------------------------
; ASMDOC Contract _mmgr_op_unk (_mmgr_compact_chunk): Move selected low chunk down over preceding vacant
; ASMDOC descriptors to previous occupied end, returning its normalized far address.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 far pointer offset word ignored; BP+8 far pointer segment used for lookup; DS=DGROUP
; ASMDOC outputs: DX:AX=resulting segment:0000, even when no move; If selected chunk was low top, resptr2 now
; ASMDOC points at relocated descriptor
; ASMDOC preserves: SI; DI; BP; DS (including copy-helper restoration); IF
; ASMDOC clobbers: AX; BX; CX; DX; ES when paragraph-copy path executes; arithmetic flags; DF cleared when
; ASMDOC copy/path helper executes
; ASMDOC globals_read: _resptr1; _resptr2; low descriptors
; ASMDOC globals_written: _resptr2; source/destination low descriptors; relocated low resource bytes
; ASMDOC services: _mmgr_copy_paras; _fatal_error shared missing-segment path
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Returns offset zero rather than preserving original input offset. Compacts this selected chunk
; ASMDOC only. Vacant descriptor size fields are not used to choose destination.
_packmemory proc far
    arg_chunk_segment = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_chunk_segment]
    mov     si, _resptr2
memory_op_loop_head_01:
    cmp     si, _resptr1
    jz      short memory_op_branch_path_01
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short memory_op_branch_path_02
    sub     si, 12h
    jmp     short memory_op_loop_head_01
memory_op_branch_path_01:
    jmp     free_chunk_loop_head_02
memory_op_branch_path_02:
    mov     di, si
    sub     di, 12h
    cmp     [di+MEMCHUNK.res_flags], 0
    jnz     short memory_op_branch_path_04
; ASMDOC Walk backward over vacant low descriptors; selected chunk relocates to previous occupied end, not to a
; ASMDOC first-fit gap.
memory_op_loop_head_02:
    sub     di, 12h
    cmp     [di+MEMCHUNK.res_flags], 0
    jz      short memory_op_loop_head_02
    mov     [si+MEMCHUNK.res_flags], 0
    mov     bx, [si+MEMCHUNK.ressize]
    push    bx
    mov     ax, [di+MEMCHUNK.resofs]
    add     ax, [di+MEMCHUNK.ressize]
    push    ax
    push    [si+MEMCHUNK.resofs]
    add     di, 12h
    cmp     si, _resptr2
    jnz     short memory_op_branch_path_03
    mov     _resptr2, di
memory_op_branch_path_03:
    mov     [di+MEMCHUNK.resofs], ax
    mov     [di+MEMCHUNK.ressize], bx
    mov     [di+MEMCHUNK.res_flags], 2
    mov     cx, 0Ch
    xor     bx, bx
memory_op_loop_head_03:
    mov     al, [bx+si+MEMCHUNK.resname]
    mov     [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    memory_op_loop_head_03
    call    _posblockmove
    add     sp, 6
memory_op_loop_head_04:
    mov     dx, [di+MEMCHUNK.resofs]
    xor     ax, ax
    pop     di
    pop     si
    pop     bp
    retf
memory_op_branch_path_04:
    mov     di, si
    jmp     short memory_op_loop_head_04
    db 0 ; retained zero fill at this boundary
_packmemory endp
S012_TEXT ends
_DATA segment word public 'DATA'
_nomemerr db 'reservememory - OUT OF MEMORY RESERVING %s P=%x HW=%x', 0Dh, 0Ah, 00h
_noblkerr db 'reservememory - OUT OF MEMORY SLOTS RESERVING %s', 0Dh, 0Ah, 00h
_blknotfound db 'memory manager - BLOCK NOT FOUND at SEG= %x', 0Dh, 0Ah, 00h
_expanderr db 'resizememory - CANNOT EXPAND BLOCK NOT AT TOP', 0Dh, 0Ah, 00h
_exmemerr db 'resizememory - NO MEMORY LEFT TO EXPAND HW=%x', 0Dh, 0Ah, 00h, 00h
_resmem_end_seg     dw 0
_resmem_base_seg     dw 0
_resmaxsize     dw 0
_pspofs     dw 0
_pspseg     dw 0
; ASMDOC Exported DATA, not callable code: 18-byte descriptors, first state 2 and final state 1 are sentinels.
; Fifty 18-byte resource slots: name=12 spaces, size=segment=0.
; State=2 at slot 0, state=1 at slot 49, otherwise 0. Verified 900/900 bytes.
_resources db '            ', 00h, 00h, 00h, 00h, 02h, 00h
    REPT 48
    db '            ', 00h, 00h, 00h, 00h, 00h, 00h
    ENDM
    db '            ', 00h, 00h, 00h, 00h, 01h, 00h
_resptr1     dw offset DGROUP:_resources
_resptr2     dw offset DGROUP:_resources
_resendptr1     dw offset DGROUP:_resources.resname+372h
_resendptr2     dw offset DGROUP:_resources.resname+372h
_DATA ends
end
