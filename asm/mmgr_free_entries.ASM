.8086
MEMCHUNK struc
    resname db 12 dup (?)
    ressize dw ?
    resofs dw ?
    resunk dw ?
MEMCHUNK ends
_TEXT segment word public 'CODE'
_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:_TEXT, ds:DGROUP
extrn _aMemoryManagerB:byte
extrn _resendptr1:word
extrn _resendptr2:word
extrn _resptr1:word
extrn _resptr2:word
extrn _fatal_error:far
extrn _copy_paras_reverse:far
extrn _mmgr_copy_paras:far
_TEXT segment word public 'CODE'
public _mmgr_free
public _nopsub_31525
public _mmgr_release
public _mmgr_get_chunk_size
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
    mov     ax, offset _aMemoryManagerB
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
    call    far ptr _copy_paras_reverse
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
    call    far ptr _mmgr_copy_paras
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
    call    far ptr _copy_paras_reverse
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
_TEXT ends
end
