.8086
S012_TEXT segment byte public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:DGROUP
MEMCHUNK struc
    resname db 12 dup (?)
    ressize dw ?
    resofs dw ?
    resunk dw ?
MEMCHUNK ends
extrn _resendptr1:word
extrn _resendptr2:word
extrn _copy_paras_reverse:far
S012_TEXT segment byte public 'CODE'
public _mmgr_find_free
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
S012_TEXT ends
end
