.8086
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
    jl      short loc_3108E
    mov     ah, 62h
    int     21h             ; DOS - 3+ - GET PSP ADDRESS
loc_3108E:
    mov     _pspofs, bx
    mov     _pspseg, ds
    cmp     _resmem_end_seg, 0
    jnz     short loc_310CD
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
loc_310CD:
    mov     si, _resendptr2
    mov     _resendptr1, si
    mov     si, _resptr1
    mov     _resptr2, si
loc_310DD:
    add     si, 12h
    cmp     si, _resendptr2
    jz      short loc_310ED
    mov     [si+MEMCHUNK.resunk], 0
    jmp     short loc_310DD
loc_310ED:
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
    jnz     short loc_310CD
    mov     bx, [bp+arg_0]
    mov     ah, 48h
    int     21h             ; DOS - 2+ - ALLOCATE MEMORY
    jnb     short loc_3113E
    mov     ah, 48h
    int     21h             ; DOS - 2+ - ALLOCATE MEMORY
loc_3113E:
    mov     si, _resptr1
    mov     [si+MEMCHUNK.resofs], ax
    mov     _resmem_base_seg, ax
    add     ax, bx
    mov     si, _resendptr2
    mov     [si+MEMCHUNK.resofs], ax
    mov     _resmem_end_seg, ax
    jmp     loc_310CD
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
loc_3119D:
    or      bx, bx
    jz      short loc_311D0
    mov     cx, 8000h
    mov     ax, bx
    sub     bx, 1000h
    jnb     short loc_311BA
    add     bx, 1000h
    shl     bx, 1
    shl     bx, 1
    shl     bx, 1
    mov     cx, bx
    xor     bx, bx
loc_311BA:
    xor     si, si
    xor     di, di
    rep movsw
    mov     ax, ds
    add     ax, 1000h
    mov     ds, ax
    mov     ax, es
    add     ax, 1000h
    mov     es, ax
    jmp     short loc_3119D
loc_311D0:
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
loc_311ED:
    or      bx, bx
    jz      short loc_31222
    mov     cx, 1000h
    sub     bx, 1000h
    jnb     short loc_31202
    add     bx, 1000h
    mov     cx, bx
    xor     bx, bx
loc_31202:
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
    jmp     short loc_311ED
loc_31222:
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
loc_31232:
    lodsb
    or      al, al
    jz      short loc_31243
    cmp     al, 3Ah ; ':'
    jz      short loc_3123F
    cmp     al, 5Ch ; '\'
    jnz     short loc_31232
loc_3123F:
    mov     bx, si
    jmp     short loc_31232
loc_31243:
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
    jbe     short loc_312A7
loc_31262:
    mov     _resptr2, di
    push    [bp+arg_0]
    call    _mmgr_path_to_name
    add     sp, 2
    mov     si, ax
    xor     bx, bx
    mov     cx, 0Ch
loc_31278:
    mov     al, byte ptr [bx+si+MEMCHUNK.resname]
    mov     byte ptr [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    loc_31278
    mov     si, _resendptr1
    mov     ax, [bp+arg_2]
    mov     [di+MEMCHUNK.resofs], dx
    mov     [di+MEMCHUNK.ressize], ax
    mov     [di+MEMCHUNK.resunk], 2
    add     ax, dx
    cmp     ax, _resmaxsize
    jb      short loc_3129C
    mov     _resmaxsize, ax
loc_3129C:
    cmp     ax, [si+MEMCHUNK.resofs]
    ja      short loc_312C2
loc_312A1:
    xor     ax, ax
    pop     di
    pop     si
    pop     bp
    retf
loc_312A7:
    cmp     si, _resendptr2
    jz      short loc_312B6
    add     si, 12h
    mov     _resendptr1, si
    jmp     short loc_31262
loc_312B6:
    push    [bp+arg_0]
    mov     ax, offset DGROUP:_aReservememoryO; "reservememory - OUT OF MEMORY SLOTS RES"...
    push    ax
    call    far ptr _fatal_error
loc_312C2:
    mov     si, _resendptr1
    mov     di, _resptr2
    mov     ax, [di+MEMCHUNK.resofs]
    add     ax, [di+MEMCHUNK.ressize]
loc_312D0:
    cmp     ax, [si+MEMCHUNK.resofs]
    jbe     short loc_312A1
    cmp     si, _resendptr2
    jz      short loc_312E9
    mov     [si+MEMCHUNK.resunk], 0
    add     si, 12h
    mov     _resendptr1, si
    jmp     short loc_312D0
loc_312E9:
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
loc_31308:
    test    [si+MEMCHUNK.resunk], 1
    jnz     short loc_31315
    add     dx, [si+MEMCHUNK.ressize]
    jmp     short loc_3131C
    db 144
loc_31315:
    or      dx, dx
    jnz     short loc_31330
loc_31319:
    sub     di, 12h
loc_3131C:
    sub     si, 12h
    cmp     si, _resendptr1
    jnb     short loc_31308
    add     di, 12h
    mov     _resendptr1, di
    pop     dx
    pop     di
    pop     si
    retf
loc_31330:
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
loc_31359:
    mov     al, byte ptr [bx+si+MEMCHUNK.resname]
    mov     byte ptr [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    loc_31359
    call    _copy_paras_reverse
    add     sp, 6
    jmp     short loc_31319
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
loc_31380:
    xor     bx, bx
    cmp     [si+MEMCHUNK.resunk], 0
    jz      short loc_313AE
loc_31388:
    mov     al, [bx+di]
    or      al, al
    jz      short loc_3139B
    cmp     al, byte ptr [bx+si+MEMCHUNK.resname]
    jnz     short loc_313A5
    inc     bx
    cmp     bx, 0Ch
    jl      short loc_31388
    jmp     short loc_313B6
    db 144
loc_3139B:
    cmp     byte ptr [bx+si+MEMCHUNK.resname], 2Eh ; '.'
    jz      short loc_313B6
    cmp     byte ptr [bx+si+MEMCHUNK.resname], 0
    jz      short loc_313B6
loc_313A5:
    add     si, 12h
    cmp     si, _resendptr2
    jb      short loc_31380
loc_313AE:
    xor     dx, dx
loc_313B0:
    xor     ax, ax
    pop     di
    pop     si
    pop     bp
    retf
loc_313B6:
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
loc_313E4:
    mov     al, byte ptr [bx+si+MEMCHUNK.resname]
    mov     byte ptr [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    loc_313E4
    cmp     di, _resendptr1
    jnz     short loc_313F6
    add     _resendptr1, 12h
loc_313F6:
    call    _mmgr_copy_paras
    add     sp, 6
    mov     si, _resendptr1
    mov     di, _resptr2
    mov     ax, [di+MEMCHUNK.resofs]
    add     ax, [di+MEMCHUNK.ressize]
loc_3140C:
    cmp     ax, [si+MEMCHUNK.resofs]
    jbe     short loc_3141F
    mov     [si+MEMCHUNK.resunk], 0
    add     si, 12h
    mov     _resendptr1, si
    jmp     short loc_3140C
loc_3141F:
    call    _mmgr_find_free
    mov     dx, [di+MEMCHUNK.resofs]
    jmp     short loc_313B0
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
loc_3143F:
    xor     bx, bx
    cmp     word ptr [si+10h], 0
    jz      short loc_3146D
loc_31447:
    mov     al, [bx+di]
    or      al, al
    jz      short loc_3145A
    cmp     al, [bx+si]
    jnz     short loc_31464
    inc     bx
    cmp     bx, 0Ch
    jl      short loc_31447
    jmp     short loc_31475
    ; align 2
    db 144
loc_3145A:
    cmp     byte ptr [bx+si], 2Eh ; '.'
    jz      short loc_31475
    cmp     byte ptr [bx+si], 0
    jz      short loc_31475
loc_31464:
    add     si, 12h
    cmp     si, _resendptr2
    jb      short loc_3143F
loc_3146D:
    xor     dx, dx
    xor     ax, ax
    pop     di
    pop     si
    pop     bp
    retf
loc_31475:
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
loc_31488:
    cmp     si, _resptr1
    jz      short loc_31498
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short loc_314A4
    sub     si, 12h
    jmp     short loc_31488
loc_31498:
    push    [bp+arg_2]
    mov     ax, offset DGROUP:_aMemoryManagerB; "memory manager - BLOCK NOT FOUND at SEG"...
    push    ax
    call    far ptr _fatal_error
loc_314A4:
    mov     [bp+arg_2], 0
    mov     [si+MEMCHUNK.resunk], 0
    cmp     si, _resptr2
    jz      short loc_314CE
    mov     bx, _resptr2
    mov     di, _resendptr1
    cmp     si, di
    jz      short loc_31508
    mov     ax, [di+MEMCHUNK.resofs]
    sub     ax, [bx+MEMCHUNK.resofs]
    sub     ax, [bx+MEMCHUNK.ressize]
    cmp     ax, [si+MEMCHUNK.ressize]
    jb      short loc_31508
loc_314CE:
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
loc_314F9:
    mov     al, byte ptr [bx+si+MEMCHUNK.resname]
    mov     byte ptr [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    loc_314F9
    call    _copy_paras_reverse
    add     sp, 6
loc_31508:
    cmp     si, _resptr2
    jnz     short loc_3151B
loc_3150E:
    sub     si, 12h
    cmp     [si+MEMCHUNK.resunk], 0
    jz      short loc_3150E
    mov     _resptr2, si
loc_3151B:
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
loc_31534:
    cmp     si, _resptr1
    jz      short loc_31544
    cmp     ax, [si+0Eh]
    jz      short loc_31547
    sub     si, 12h
    jmp     short loc_31534
loc_31544:
    jmp     loc_31498
loc_31547:
    mov     [bp+arg_2], 0
    mov     word ptr [si+10h], 0
    cmp     si, _resptr2
    jz      short loc_31570
    mov     bx, _resptr2
    mov     di, _resendptr2
    mov     ax, [di+0Eh]
    sub     ax, [bx+0Eh]
    sub     ax, [bx+0Ch]
    cmp     ax, [si+0Ch]
    ja      short loc_31570
    jmp     loc_31622
loc_31570:
    mov     di, _resptr2
    mov     ax, [di+0Eh]
    add     ax, [di+0Ch]
    mov     [bp+var_2], ax
    mov     [bp+var_4], 0
    mov     di, _resendptr1
loc_31586:
    cmp     di, _resendptr2
    jz      short loc_315E9
    mov     ax, [di+0Eh]
    sub     ax, [si+0Ch]
    cmp     [bp+var_2], ax
    ja      short loc_315E4
    mov     bx, di
    sub     bx, 12h
    cmp     bx, _resptr2
    jz      short loc_315E4
    cmp     [bp+var_4], 0
    jnz     short loc_315B1
    mov     _resendptr1, bx
    mov     [bp+var_4], 1
loc_315B1:
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
loc_315CE:
    mov     al, [di]
    mov     [bx], al
    inc     bx
    inc     di
    loop    loc_315CE
    sub     di, 0Ch
    sub     bx, 0Ch
    call    _mmgr_copy_paras
    add     sp, 6
loc_315E4:
    add     di, 12h
    jmp     short loc_31586
loc_315E9:
    mov     ax, [di+0Eh]
    sub     di, 12h
    cmp     [bp+var_4], 0
    jnz     short loc_315F9
    mov     _resendptr1, di
loc_315F9:
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
loc_31613:
    mov     al, [bx+si]
    mov     [bx+di], al
    inc     bx
    loop    loc_31613
    call    _copy_paras_reverse
    add     sp, 6
loc_31622:
    cmp     si, _resptr2
    jnz     short loc_31635
loc_31628:
    sub     si, 12h
    cmp     word ptr [si+10h], 0
    jz      short loc_31628
    mov     _resptr2, si
loc_31635:
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
loc_3164D:
    cmp     si, _resptr1
    jz      short loc_3165D
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short loc_31660
    sub     si, 12h
    jmp     short loc_3164D
loc_3165D:
    jmp     loc_31498
loc_31660:
    mov     [si+MEMCHUNK.resunk], 0
    cmp     si, _resptr2
    jnz     short loc_31678
loc_3166B:
    sub     si, 12h
    cmp     [si+MEMCHUNK.resunk], 0
    jz      short loc_3166B
    mov     _resptr2, si
loc_31678:
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
loc_31688:
    cmp     si, _resptr1
    jz      short loc_31698
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short loc_3169B
    sub     si, 12h
    jmp     short loc_31688
loc_31698:
    jmp     loc_31498
loc_3169B:
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
loc_316AE:
    cmp     si, _resptr1
    jz      short loc_316BE
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short loc_316C1
    sub     si, 12h
    jmp     short loc_316AE
loc_316BE:
    jmp     loc_31498
loc_316C1:
    mov     ax, [bp+arg_4]
    cmp     ax, [si+MEMCHUNK.ressize]
    ja      short loc_316D0
    mov     [si+MEMCHUNK.ressize], ax
    pop     di
    pop     si
    pop     bp
    retf
loc_316D0:
    cmp     si, _resptr2
    jnz     short loc_316F4
    mov     [si+MEMCHUNK.ressize], ax
    mov     di, _resendptr1
    add     ax, [si+MEMCHUNK.resofs]
    cmp     ax, _resmaxsize
    jb      short loc_316E9
    mov     _resmaxsize, ax
loc_316E9:
    cmp     ax, [di+MEMCHUNK.resofs]
    ja      short loc_316FD
loc_316EE:
    xor     ax, ax
    pop     di
    pop     si
    pop     bp
    retf
loc_316F4:
    mov     ax, offset DGROUP:_aResizememoryCa; "resizememory - CANNOT EXPAND BLOCK NOT "...
    push    ax
    call    far ptr _fatal_error
loc_316FD:
    mov     si, _resendptr1
    mov     di, _resptr2
    mov     ax, [di+MEMCHUNK.resofs]
    add     ax, [di+MEMCHUNK.ressize]
loc_3170B:
    cmp     ax, [si+MEMCHUNK.resofs]
    jbe     short loc_316EE
    cmp     si, _resendptr2
    jz      short loc_31724
    mov     [si+MEMCHUNK.resunk], 0
    add     si, 12h
    mov     _resendptr1, si
    jmp     short loc_3170B
loc_31724:
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
loc_3173E:
    cmp     si, _resptr1
    jz      short loc_3174E
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short loc_31751
    sub     si, 12h
    jmp     short loc_3173E
loc_3174E:
    jmp     loc_31498
loc_31751:
    mov     di, si
    sub     di, 12h
    cmp     [di+MEMCHUNK.resunk], 0
    jnz     short loc_317AD
loc_3175C:
    sub     di, 12h
    cmp     [di+MEMCHUNK.resunk], 0
    jz      short loc_3175C
    mov     [si+MEMCHUNK.resunk], 0
    mov     bx, [si+MEMCHUNK.ressize]
    push    bx
    mov     ax, [di+MEMCHUNK.resofs]
    add     ax, [di+MEMCHUNK.ressize]
    push    ax
    push    [si+MEMCHUNK.resofs]
    add     di, 12h
    cmp     si, _resptr2
    jnz     short loc_31785
    mov     _resptr2, di
loc_31785:
    mov     [di+MEMCHUNK.resofs], ax
    mov     [di+MEMCHUNK.ressize], bx
    mov     [di+MEMCHUNK.resunk], 2
    mov     cx, 0Ch
    xor     bx, bx
loc_31795:
    mov     al, byte ptr [bx+si+MEMCHUNK.resname]
    mov     byte ptr [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    loc_31795
    call    _mmgr_copy_paras
    add     sp, 6
loc_317A4:
    mov     dx, [di+MEMCHUNK.resofs]
    xor     ax, ax
    pop     di
    pop     si
    pop     bp
    retf
loc_317AD:
    mov     di, si
    jmp     short loc_317A4
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
