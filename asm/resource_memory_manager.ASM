.8086
name asm012_135290
MEMCHUNK struc
resname db 12 dup (?)
ressize dw ?
resofs dw ?
resunk dw ?
MEMCHUNK ends
DSEG segment byte public 'STUNTSD'
DSEG ends
_DATA segment word public 'DATA'
_DATA ends
DGROUP group _DATA, DSEG
assume ds:DGROUP
extrn _fatal_error:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _mmgr_alloc_resmem
public _mmgr_alloc_a000
public _nopsub_310FE
public _nopsub_3111D
public _nopsub_31157
public _nopsub_31169
public _mmgr_get_ofs_diff
public _mmgr_copy_paras
public _copy_paras_reverse
public _mmgr_path_to_name
public _mmgr_alloc_pages
public _mmgr_find_free
public _mmgr_get_chunk_by_name
public _nopsub_31429
public _mmgr_free
public _nopsub_31525
public _mmgr_release
public _mmgr_get_chunk_size
public _mmgr_resize_memory
public _mmgr_op_unk
public _aReservememoryOutOfMemory, _aResizememoryCa, _aResizememoryNo, _resources
_mmgr_alloc_resmem proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, 3000h
    int     21h             ; DOS - GET DOS VERSION
    mov     bx, cs
    cmp     al, 3
    jl      short resmem_alloc_branch_path_01
    mov     ah, 62h
    int     21h             ; DOS - 3+ - GET PSP ADDRESS
resmem_alloc_branch_path_01:
    mov     _pspofs, bx
    mov     _pspseg, ds
    cmp     _resmem_end_seg, 0
    jnz     short resmem_alloc_loop_head_01
    mov     bx, 64h ; 'd'
    mov     ah, 48h
    int     21h             ; DOS - 2+ - ALLOCATE MEMORY
    mov     si, _resptr1
    mov     [si+MEMCHUNK.resofs], ax
    mov     _resmem_base_seg, ax
    mov     es, ax
    mov     bx, [bp+arg_0]
    sub     bx, [si+MEMCHUNK.resofs]
    mov     ah, 4Ah
    int     21h             ; DOS - 2+ - ADJUST MEMORY BLOCK SIZE (SETBLOCK)
    mov     ah, 4Ah
    int     21h             ; DOS - 2+ - ADJUST MEMORY BLOCK SIZE (SETBLOCK)
    mov     ax, _resmem_base_seg
    add     ax, bx
    mov     si, _resendptr2
    mov     [si+MEMCHUNK.resofs], ax
    mov     _resmem_end_seg, ax
resmem_alloc_loop_head_01:
    mov     si, _resendptr2
    mov     _resendptr1, si
    mov     si, _resptr1
    mov     _resptr2, si
resmem_alloc_loop_head_02:
    add     si, 12h
    cmp     si, _resendptr2
    jz      short resmem_alloc_branch_path_02
    mov     [si+MEMCHUNK.resunk], 0
    jmp     short resmem_alloc_loop_head_02
resmem_alloc_branch_path_02:
    pop     di
    pop     si
    pop     bp
    retf
_mmgr_alloc_resmem endp
_mmgr_alloc_a000 proc far

    mov     ax, 0A000h
    push    ax
    call    _mmgr_alloc_resmem
    add     sp, 2
    retf
_mmgr_alloc_a000 endp
_nopsub_310FE proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, 0A000h
    push    ax
    call    _mmgr_alloc_resmem
    add     sp, 2
    mov     ax, [bp+arg_0]
    mov     bx, _resendptr2
    sub     [bx+MEMCHUNK.resofs], ax
    sub     _resmem_end_seg, ax
    pop     bp
    retf
_nopsub_310FE endp
_nopsub_3111D proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     _pspofs, cs
    mov     _pspseg, ds
    cmp     _resmem_end_seg, 0
    jnz     short resmem_alloc_loop_head_01
    mov     bx, [bp+arg_0]
    mov     ah, 48h
    int     21h             ; DOS - 2+ - ALLOCATE MEMORY
    jnb     short noop_entry2_branch_path_01
    mov     ah, 48h
    int     21h             ; DOS - 2+ - ALLOCATE MEMORY
noop_entry2_branch_path_01:
    mov     si, _resptr1
    mov     [si+MEMCHUNK.resofs], ax
    mov     _resmem_base_seg, ax
    add     ax, bx
    mov     si, _resendptr2
    mov     [si+MEMCHUNK.resofs], ax
    mov     _resmem_end_seg, ax
    jmp     resmem_alloc_loop_head_01
_nopsub_3111D endp
_nopsub_31157 proc far

    mov     bx, _resendptr1
    mov     ax, [bx+MEMCHUNK.resofs]
    mov     bx, _resptr2
    sub     ax, [bx+MEMCHUNK.resofs]
    sub     ax, [bx+MEMCHUNK.ressize]
    retf
_nopsub_31157 endp
_nopsub_31169 proc far

    mov     bx, _resptr2
    mov     ax, [bx+MEMCHUNK.resofs]
    add     ax, [bx+MEMCHUNK.ressize]
    mov     bx, _resptr1
    sub     ax, [bx+MEMCHUNK.resofs]
    retf
_nopsub_31169 endp
_mmgr_get_ofs_diff proc far

    mov     bx, _resendptr2
    mov     ax, [bx+MEMCHUNK.resofs]
    mov     bx, _resptr2
    sub     ax, [bx+MEMCHUNK.resofs]
    sub     ax, [bx+MEMCHUNK.ressize]
    retf
_mmgr_get_ofs_diff endp
_mmgr_copy_paras proc far
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
    cld
    mov     ds, [bp+arg_0]
    mov     es, [bp+arg_2]
    mov     bx, [bp+arg_4]
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
_mmgr_copy_paras endp
_copy_paras_reverse proc far
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
    std
    mov     bx, [bp+arg_4]
    mov     ax, [bp+arg_0]
    add     ax, bx
    mov     ds, ax
    mov     ax, [bp+arg_2]
    add     ax, bx
    mov     es, ax
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
_copy_paras_reverse endp
_mmgr_path_to_name proc far
     s = byte ptr 0
     r = byte ptr 2
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
_mmgr_alloc_pages proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

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
    push    [bp+arg_0]
    call    _mmgr_path_to_name
    add     sp, 2
    mov     si, ax
    xor     bx, bx
    mov     cx, 0Ch
pages_alloc_loop_head_02:
    mov     al, byte ptr [bx+si+MEMCHUNK.resname]
    mov     byte ptr [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    pages_alloc_loop_head_02
    mov     si, _resendptr1
    mov     ax, [bp+arg_2]
    mov     [di+MEMCHUNK.resofs], dx
    mov     [di+MEMCHUNK.ressize], ax
    mov     [di+MEMCHUNK.resunk], 2
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
    push    [bp+arg_0]
    mov     ax, offset DGROUP:_aReservememoryO; "reservememory - OUT OF MEMORY SLOTS RES"...
    push    ax
    call    far ptr _fatal_error
pages_alloc_branch_path_03:
    mov     si, _resendptr1
    mov     di, _resptr2
    mov     ax, [di+MEMCHUNK.resofs]
    add     ax, [di+MEMCHUNK.ressize]
pages_alloc_loop_head_04:
    cmp     ax, [si+MEMCHUNK.resofs]
    jbe     short pages_alloc_loop_head_03
    cmp     si, _resendptr2
    jz      short pages_alloc_branch_path_04
    mov     [si+MEMCHUNK.resunk], 0
    add     si, 12h
    mov     _resendptr1, si
    jmp     short pages_alloc_loop_head_04
pages_alloc_branch_path_04:
    mov     bx, _resmaxsize
    push    bx
    push    [di+MEMCHUNK.ressize]
    push    [bp+arg_0]
    mov     ax, offset DGROUP:_aReservememoryOutOfMemory; "reservememory - OUT OF MEMORY RESERVING"...
    push    ax
    call    far ptr _fatal_error
_mmgr_alloc_pages endp
_mmgr_find_free proc far
     r = byte ptr 0

    push    si
    push    di
    push    dx
    mov     si, _resendptr2
    mov     di, si
    xor     dx, dx
find_free_loop_head_01:
    test    [si+MEMCHUNK.resunk], 1
    jnz     short find_free_branch_path_01
    add     dx, [si+MEMCHUNK.ressize]
    jmp     short find_free_join_01
    db 144
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
    mov     cx, [si+MEMCHUNK.resunk]
    mov     [si+MEMCHUNK.resunk], 0
    mov     [di+MEMCHUNK.resunk], cx
    xor     bx, bx
    mov     cx, 0Ch
find_free_loop_head_03:
    mov     al, byte ptr [bx+si+MEMCHUNK.resname]
    mov     byte ptr [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    find_free_loop_head_03
    call    _copy_paras_reverse
    add     sp, 6
    jmp     short find_free_loop_head_02
_mmgr_find_free endp
_mmgr_get_chunk_by_name proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     si, _resendptr1
    push    [bp+arg_0]
    call    _mmgr_path_to_name
    add     sp, 2
    mov     di, ax
find_chunk_loop_head_01:
    xor     bx, bx
    cmp     [si+MEMCHUNK.resunk], 0
    jz      short find_chunk_branch_path_03
find_chunk_loop_head_02:
    mov     al, [bx+di]
    or      al, al
    jz      short find_chunk_branch_path_01
    cmp     al, byte ptr [bx+si+MEMCHUNK.resname]
    jnz     short find_chunk_branch_path_02
    inc     bx
    cmp     bx, 0Ch
    jl      short find_chunk_loop_head_02
    jmp     short find_chunk_branch_path_04
    db 144
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
    mov     [si+MEMCHUNK.resunk], 0
    mov     [di+MEMCHUNK.resofs], dx
    mov     [di+MEMCHUNK.ressize], ax
    mov     [di+MEMCHUNK.resunk], 2
    xor     bx, bx
    mov     cx, 0Ch
find_chunk_loop_head_04:
    mov     al, byte ptr [bx+si+MEMCHUNK.resname]
    mov     byte ptr [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    find_chunk_loop_head_04
    cmp     di, _resendptr1
    jnz     short find_chunk_branch_path_05
    add     _resendptr1, 12h
find_chunk_branch_path_05:
    call    _mmgr_copy_paras
    add     sp, 6
    mov     si, _resendptr1
    mov     di, _resptr2
    mov     ax, [di+MEMCHUNK.resofs]
    add     ax, [di+MEMCHUNK.ressize]
find_chunk_loop_head_05:
    cmp     ax, [si+MEMCHUNK.resofs]
    jbe     short find_chunk_branch_path_06
    mov     [si+MEMCHUNK.resunk], 0
    add     si, 12h
    mov     _resendptr1, si
    jmp     short find_chunk_loop_head_05
find_chunk_branch_path_06:
    call    _mmgr_find_free
    mov     dx, [di+MEMCHUNK.resofs]
    jmp     short find_chunk_loop_head_03
_mmgr_get_chunk_by_name endp
_nopsub_31429 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     si, _resendptr1
    push    [bp+arg_0]
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
    ; align 2
    db 144
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
_nopsub_31429 endp
_mmgr_free proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_2]
    mov     si, _resptr2
free_chunk_loop_head_01:
    cmp     si, _resptr1
    jz      short free_chunk_loop_head_02
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short free_chunk_branch_path_01
    sub     si, 12h
    jmp     short free_chunk_loop_head_01
free_chunk_loop_head_02:
    push    [bp+arg_2]
    mov     ax, offset DGROUP:_aMemoryManagerB; "memory manager - BLOCK NOT FOUND at SEG"...
    push    ax
    call    far ptr _fatal_error
free_chunk_branch_path_01:
    mov     [bp+arg_2], 0
    mov     [si+MEMCHUNK.resunk], 0
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
free_chunk_branch_path_02:
    mov     bx, [si+MEMCHUNK.ressize]
    push    bx
    mov     di, _resendptr1
    mov     ax, [di+MEMCHUNK.resofs]
    sub     ax, bx
    push    ax
    mov     [bp+arg_2], ax
    push    [si+MEMCHUNK.resofs]
    sub     di, 12h
    mov     _resendptr1, di
    mov     [di+MEMCHUNK.resofs], ax
    mov     [di+MEMCHUNK.ressize], bx
    mov     [di+MEMCHUNK.resunk], 1
    mov     cx, 0Ch
    xor     bx, bx
free_chunk_loop_head_03:
    mov     al, byte ptr [bx+si+MEMCHUNK.resname]
    mov     byte ptr [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    free_chunk_loop_head_03
    call    _copy_paras_reverse
    add     sp, 6
free_chunk_branch_path_03:
    cmp     si, _resptr2
    jnz     short free_chunk_branch_path_04
free_chunk_loop_head_04:
    sub     si, 12h
    cmp     [si+MEMCHUNK.resunk], 0
    jz      short free_chunk_loop_head_04
    mov     _resptr2, si
free_chunk_branch_path_04:
    mov     ax, [bp+arg_0]
    mov     dx, [bp+arg_2]
    pop     di
    pop     si
    pop     bp
    retf
_mmgr_free endp
_nopsub_31525 proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 6
    push    si
    push    di
    mov     ax, [bp+arg_2]
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
    mov     [bp+arg_2], 0
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
    call    _mmgr_copy_paras
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
    call    _copy_paras_reverse
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
    mov     ax, [bp+arg_0]
    mov     dx, [bp+arg_2]
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
_nopsub_31525 endp
_mmgr_release proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_2]
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
    mov     [si+MEMCHUNK.resunk], 0
    cmp     si, _resptr2
    jnz     short release_chunk_branch_path_03
release_chunk_loop_head_02:
    sub     si, 12h
    cmp     [si+MEMCHUNK.resunk], 0
    jz      short release_chunk_loop_head_02
    mov     _resptr2, si
release_chunk_branch_path_03:
    pop     di
    pop     si
    pop     bp
    retf
_mmgr_release endp
_mmgr_get_chunk_size proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_2]
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
_mmgr_get_chunk_size endp
_mmgr_resize_memory proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 8
    arg_4 = word ptr 10

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_2]
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
    mov     ax, [bp+arg_4]
    cmp     ax, [si+MEMCHUNK.ressize]
    ja      short resize_mem_branch_path_03
    mov     [si+MEMCHUNK.ressize], ax
    pop     di
    pop     si
    pop     bp
    retf
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
    mov     ax, offset DGROUP:_aResizememoryCa; "resizememory - CANNOT EXPAND BLOCK NOT "...
    push    ax
    call    far ptr _fatal_error
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
    mov     [si+MEMCHUNK.resunk], 0
    add     si, 12h
    mov     _resendptr1, si
    jmp     short resize_mem_loop_head_03
resize_mem_error_path_02:
    mov     bx, _resmaxsize
    push    bx
    mov     ax, offset DGROUP:_aResizememoryNo; "resizememory - NO MEMORY LEFT TO EXPAND"...
    push    ax
    call    far ptr _fatal_error
_mmgr_resize_memory endp
_mmgr_op_unk proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_2]
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
    cmp     [di+MEMCHUNK.resunk], 0
    jnz     short memory_op_branch_path_04
memory_op_loop_head_02:
    sub     di, 12h
    cmp     [di+MEMCHUNK.resunk], 0
    jz      short memory_op_loop_head_02
    mov     [si+MEMCHUNK.resunk], 0
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
    mov     [di+MEMCHUNK.resunk], 2
    mov     cx, 0Ch
    xor     bx, bx
memory_op_loop_head_03:
    mov     al, byte ptr [bx+si+MEMCHUNK.resname]
    mov     byte ptr [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    memory_op_loop_head_03
    call    _mmgr_copy_paras
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
    ; align 2
    db 0
_mmgr_op_unk endp
S012_TEXT ends
_DATA segment word public 'DATA'
_aReservememoryOutOfMemory     db 114
    db 101
    db 115
    db 101
    db 114
    db 118
    db 101
    db 109
    db 101
    db 109
    db 111
    db 114
    db 121
    db 32
    db 45
    db 32
    db 79
    db 85
    db 84
    db 32
    db 79
    db 70
    db 32
    db 77
    db 69
    db 77
    db 79
    db 82
    db 89
    db 32
    db 82
    db 69
    db 83
    db 69
    db 82
    db 86
    db 73
    db 78
    db 71
    db 32
    db 37
    db 115
    db 32
    db 80
    db 61
    db 37
    db 120
    db 32
    db 72
    db 87
    db 61
    db 37
    db 120
    db 13
    db 10
    db 0
_aReservememoryO     db 114
    db 101
    db 115
    db 101
    db 114
    db 118
    db 101
    db 109
    db 101
    db 109
    db 111
    db 114
    db 121
    db 32
    db 45
    db 32
    db 79
    db 85
    db 84
    db 32
    db 79
    db 70
    db 32
    db 77
    db 69
    db 77
    db 79
    db 82
    db 89
    db 32
    db 83
    db 76
    db 79
    db 84
    db 83
    db 32
    db 82
    db 69
    db 83
    db 69
    db 82
    db 86
    db 73
    db 78
    db 71
    db 32
    db 37
    db 115
    db 13
    db 10
    db 0
_aMemoryManagerB     db 109
    db 101
    db 109
    db 111
    db 114
    db 121
    db 32
    db 109
    db 97
    db 110
    db 97
    db 103
    db 101
    db 114
    db 32
    db 45
    db 32
    db 66
    db 76
    db 79
    db 67
    db 75
    db 32
    db 78
    db 79
    db 84
    db 32
    db 70
    db 79
    db 85
    db 78
    db 68
    db 32
    db 97
    db 116
    db 32
    db 83
    db 69
    db 71
    db 61
    db 32
    db 37
    db 120
    db 13
    db 10
    db 0
_aResizememoryCa     db 114
    db 101
    db 115
    db 105
    db 122
    db 101
    db 109
    db 101
    db 109
    db 111
    db 114
    db 121
    db 32
    db 45
    db 32
    db 67
    db 65
    db 78
    db 78
    db 79
    db 84
    db 32
    db 69
    db 88
    db 80
    db 65
    db 78
    db 68
    db 32
    db 66
    db 76
    db 79
    db 67
    db 75
    db 32
    db 78
    db 79
    db 84
    db 32
    db 65
    db 84
    db 32
    db 84
    db 79
    db 80
    db 13
    db 10
    db 0
_aResizememoryNo     db 114
    db 101
    db 115
    db 105
    db 122
    db 101
    db 109
    db 101
    db 109
    db 111
    db 114
    db 121
    db 32
    db 45
    db 32
    db 78
    db 79
    db 32
    db 77
    db 69
    db 77
    db 79
    db 82
    db 89
    db 32
    db 76
    db 69
    db 70
    db 84
    db 32
    db 84
    db 79
    db 32
    db 69
    db 88
    db 80
    db 65
    db 78
    db 68
    db 32
    db 72
    db 87
    db 61
    db 37
    db 120
    db 13
    db 10
    db 0
    db 0
_resmem_end_seg     dw 0
_resmem_base_seg     dw 0
_resmaxsize     dw 0
_pspofs     dw 0
_pspseg     dw 0
_resources     db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 2
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 32
    db 0
    db 0
    db 0
    db 0
    db 1
    db 0
_resptr1     dw offset DGROUP:_resources
_resptr2     dw offset DGROUP:_resources
_resendptr1     dw offset DGROUP:_resources.resname+372h
_resendptr2     dw offset DGROUP:_resources.resname+372h
_DATA ends
end
