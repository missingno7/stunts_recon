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
extrn _pspofs:word
extrn _pspseg:word
extrn _resendptr1:word
extrn _resendptr2:word
extrn _resptr1:word
extrn _resptr2:word
extrn _word_3ff82:word
extrn _word_3ff84:word
S012_TEXT segment word public 'CODE'
public _mmgr_alloc_resmem
public _mmgr_alloc_a000
public _nopsub_310FE
public _nopsub_3111D
_mmgr_alloc_resmem proc far
     mmgr_alloc_resmem_s = byte ptr 0
     mmgr_alloc_resmem_r = byte ptr 2
    mmgr_alloc_resmem_arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, 3000h
    int     21h
    mov     bx, cs
    cmp     al, 3
    jl      short loc_3108E
    mov     ah, 62h
    int     21h
loc_3108E:
    mov     _pspofs, bx
    mov     _pspseg, ds
    cmp     _word_3ff82, 0
    jnz     short loc_310CD
    mov     bx, 64h
    mov     ah, 48h
    int     21h
    mov     si, _resptr1
    mov     [si+MEMCHUNK.resofs], ax
    mov     _word_3ff84, ax
    mov     es, ax
    mov     bx, [bp+mmgr_alloc_resmem_arg_0]
    sub     bx, [si+MEMCHUNK.resofs]
    mov     ah, 4Ah
    int     21h
    mov     ah, 4Ah
    int     21h
    mov     ax, _word_3ff84
    add     ax, bx
    mov     si, _resendptr2
    mov     [si+MEMCHUNK.resofs], ax
    mov     _word_3ff82, ax
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
     nopsub_310FE_s = byte ptr 0
     nopsub_310FE_r = byte ptr 2
    nopsub_310FE_arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, 0A000h
    push    ax
    call    _mmgr_alloc_resmem
    add     sp, 2
    mov     ax, [bp+nopsub_310FE_arg_0]
    mov     bx, _resendptr2
    sub     [bx+MEMCHUNK.resofs], ax
    sub     _word_3ff82, ax
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
    cmp     _word_3FF82, 0
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
    mov     _word_3FF84, ax
    add     ax, bx
    mov     si, _resendptr2
    mov     [si+MEMCHUNK.resofs], ax
    mov     _word_3FF82, ax
    jmp     loc_310CD
_nopsub_3111D endp
S012_TEXT ends
end
