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
chunk_scan_entry_loop:
    xor     bx, bx
    cmp     [si+MEMCHUNK.resunk], 0
    jz      short chunk_not_found
chunk_compare_name_bytes:
    mov     al, [bx+di]
    or      al, al
    jz      short chunk_name_ended
    cmp     al, byte ptr [bx+si+MEMCHUNK.resname]
    jnz     short chunk_advance_candidate
    inc     bx
    cmp     bx, 0Ch
    jl      short chunk_compare_name_bytes
    jmp     short chunk_match_found
    db 144
chunk_name_ended:
    cmp     byte ptr [bx+si+MEMCHUNK.resname], 2Eh ; '.'
    jz      short chunk_match_found
    cmp     byte ptr [bx+si+MEMCHUNK.resname], 0
    jz      short chunk_match_found
chunk_advance_candidate:
    add     si, 12h
    cmp     si, _resendptr2
    jb      short chunk_scan_entry_loop
chunk_not_found:
    xor     dx, dx
chunk_return_no_match:
    xor     ax, ax
    pop     di
    pop     si
    pop     bp
    retf
chunk_match_found:
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
chunk_copy_name_loop:
    mov     al, byte ptr [bx+si+MEMCHUNK.resname]
    mov     byte ptr [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    chunk_copy_name_loop
    cmp     di, _resendptr1
    jnz     short chunk_update_free_list_head
    add     _resendptr1, 12h
chunk_update_free_list_head:
    call    _mmgr_copy_paras
    add     sp, 6
    mov     si, _resendptr1
    mov     di, _resptr2
    mov     ax, [di+MEMCHUNK.resofs]
    add     ax, [di+MEMCHUNK.ressize]
chunk_skip_consumed_loop:
    cmp     ax, [si+MEMCHUNK.resofs]
    jbe     short chunk_find_free_after_move
    mov     [si+MEMCHUNK.resunk], 0
    add     si, 12h
    mov     _resendptr1, si
    jmp     short chunk_skip_consumed_loop
chunk_find_free_after_move:
    call    _mmgr_find_free
    mov     dx, [di+MEMCHUNK.resofs]
    jmp     short chunk_return_no_match
_mmgr_get_chunk_by_name endp
S012_TEXT ends
end
