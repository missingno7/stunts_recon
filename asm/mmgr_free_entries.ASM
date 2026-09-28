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
free_search_entry_loop:
    cmp     si, _resptr1
    jz      short free_not_found
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short free_mark_chunk_released
    sub     si, 12h
    jmp     short free_search_entry_loop
free_not_found:
    push    [bp+arg_2]
    mov     ax, offset _aMemoryManagerB
    push    ax
    call    far ptr _fatal_error
free_mark_chunk_released:
    mov     [bp+arg_2], 0
    mov     [si+MEMCHUNK.resunk], 0
    cmp     si, _resptr2
    jz      short free_relocate_released_block
    mov     bx, _resptr2
    mov     di, _resendptr1
    cmp     si, di
    jz      short free_after_optional_move
    mov     ax, [di+MEMCHUNK.resofs]
    sub     ax, [bx+MEMCHUNK.resofs]
    sub     ax, [bx+MEMCHUNK.ressize]
    cmp     ax, [si+MEMCHUNK.ressize]
    jb      short free_after_optional_move
free_relocate_released_block:
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
free_copy_name_loop:
    mov     al, byte ptr [bx+si+MEMCHUNK.resname]
    mov     byte ptr [bx+di+MEMCHUNK.resname], al
    inc     bx
    loop    free_copy_name_loop
    call    far ptr _copy_paras_reverse
    add     sp, 6
free_after_optional_move:
    cmp     si, _resptr2
    jnz     short free_update_end_pointer
free_trim_free_tail_loop:
    sub     si, 12h
    cmp     [si+MEMCHUNK.resunk], 0
    jz      short free_trim_free_tail_loop
    mov     _resptr2, si
free_update_end_pointer:
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
compact_search_entry_loop:
    cmp     si, _resptr1
    jz      short compact_reuse_free_failure
    cmp     ax, [si+0Eh]
    jz      short compact_mark_chunk_released
    sub     si, 12h
    jmp     short compact_search_entry_loop
compact_reuse_free_failure:
    jmp     free_not_found
compact_mark_chunk_released:
    mov     [bp+arg_2], 0
    mov     word ptr [si+10h], 0
    cmp     si, _resptr2
    jz      short compact_check_space
    mov     bx, _resptr2
    mov     di, _resendptr2
    mov     ax, [di+0Eh]
    sub     ax, [bx+0Eh]
    sub     ax, [bx+0Ch]
    cmp     ax, [si+0Ch]
    ja      short compact_check_space
    jmp     compact_trim_free_tail
compact_check_space:
    mov     di, _resptr2
    mov     ax, [di+0Eh]
    add     ax, [di+0Ch]
    mov     [bp+var_2], ax
    mov     [bp+var_4], 0
    mov     di, _resendptr1
compact_scan_entries_to_shift:
    cmp     di, _resendptr2
    jz      short compact_finish_shift_scan
    mov     ax, [di+0Eh]
    sub     ax, [si+0Ch]
    cmp     [bp+var_2], ax
    ja      short compact_continue_entry_scan
    mov     bx, di
    sub     bx, 12h
    cmp     bx, _resptr2
    jz      short compact_continue_entry_scan
    cmp     [bp+var_4], 0
    jnz     short compact_begin_shift
    mov     _resendptr1, bx
    mov     [bp+var_4], 1
compact_begin_shift:
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
compact_copy_entry_name_loop:
    mov     al, [di]
    mov     [bx], al
    inc     bx
    inc     di
    loop    compact_copy_entry_name_loop
    sub     di, 0Ch
    sub     bx, 0Ch
    call    far ptr _mmgr_copy_paras
    add     sp, 6
compact_continue_entry_scan:
    add     di, 12h
    jmp     short compact_scan_entries_to_shift
compact_finish_shift_scan:
    mov     ax, [di+0Eh]
    sub     di, 12h
    cmp     [bp+var_4], 0
    jnz     short compact_create_free_entry
    mov     _resendptr1, di
compact_create_free_entry:
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
compact_copy_chunk_name_loop:
    mov     al, [bx+si]
    mov     [bx+di], al
    inc     bx
    loop    compact_copy_chunk_name_loop
    call    far ptr _copy_paras_reverse
    add     sp, 6
compact_trim_free_tail:
    cmp     si, _resptr2
    jnz     short compact_update_end_pointer
compact_trim_tail_loop:
    sub     si, 12h
    cmp     word ptr [si+10h], 0
    jz      short compact_trim_tail_loop
    mov     _resptr2, si
compact_update_end_pointer:
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
release_search_entry_loop:
    cmp     si, _resptr1
    jz      short release_not_found
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short release_mark_chunk_released
    sub     si, 12h
    jmp     short release_search_entry_loop
release_not_found:
    jmp     free_not_found
release_mark_chunk_released:
    mov     [si+MEMCHUNK.resunk], 0
    cmp     si, _resptr2
    jnz     short release_return
release_trim_free_tail_loop:
    sub     si, 12h
    cmp     [si+MEMCHUNK.resunk], 0
    jz      short release_trim_free_tail_loop
    mov     _resptr2, si
release_return:
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
size_search_entry_loop:
    cmp     si, _resptr1
    jz      short size_not_found
    cmp     ax, [si+MEMCHUNK.resofs]
    jz      short size_return_chunk_bytes
    sub     si, 12h
    jmp     short size_search_entry_loop
size_not_found:
    jmp     free_not_found
size_return_chunk_bytes:
    mov     ax, [si+MEMCHUNK.ressize]
    pop     di
    pop     si
    pop     bp
    retf
_mmgr_get_chunk_size endp
_TEXT ends
end
