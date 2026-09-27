.8086
S012_TEXT segment word public 'CODE'
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
extrn _fatal_error:far
extrn _mmgr_path_to_name:far
extrn _aReservememoryOutOfMemory:byte
extrn _aReservememoryO:byte
extrn _resmaxsize:word
extrn _resptr2:word
extrn _resendptr1:word
extrn _resendptr2:word
S012_TEXT segment word public 'CODE'
public _mmgr_alloc_pages
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
    mov     ax, offset _aReservememoryO; "reservememory - OUT OF MEMORY SLOTS RES"...
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
    mov     ax, offset _aReservememoryOutOfMemory; "reservememory - OUT OF MEMORY RESERVING"...
    push    ax
    call    far ptr _fatal_error
_mmgr_alloc_pages endp
S012_TEXT ends
end
