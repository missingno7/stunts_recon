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
extrn _mmgr_copy_paras:far
extrn _mmgr_path_to_name:far
extrn _mmgr_find_free:far
extrn _resptr2:word
extrn _resendptr1:word
extrn _resendptr2:word
S012_TEXT segment word public 'CODE'
public _mmgr_get_chunk_by_name
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
S012_TEXT ends
end
