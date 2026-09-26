.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn _mmgr_get_chunk_by_name:far
extrn _file_paras:far
extrn _mmgr_alloc_pages:far
extrn _file_read:far
public _file_load_binary
public _file_load_binary_nofatal
_file_load_binary proc far
    push bp
    mov bp, sp
    sub sp, 2
    mov ax, [bp+8]
    mov [bp-2], ax
    jmp short common_entry
    nop
_file_load_binary endp
_file_load_binary_nofatal proc far
    push bp
    mov bp, sp
    sub sp, 2
    mov word ptr [bp-2], 0
    jmp short common_entry
    nop
fatal_entry:
    push bp
    mov bp, sp
    sub sp, 2
    mov word ptr [bp-2], 1
common_entry:
    push word ptr [bp+6]
    call far ptr _mmgr_get_chunk_by_name
    add sp, 2
    or dx, dx
    jnz load_complete
    push word ptr [bp-2]
    push word ptr [bp+6]
    call far ptr _file_paras
    add sp, 4
    or ax, ax
    jz empty_file
    push ax
    push word ptr [bp+6]
    call far ptr _mmgr_alloc_pages
    add sp, 4
    push word ptr [bp-2]
    push dx
    push ax
    push word ptr [bp+6]
    call far ptr _file_read
    add sp, 8
load_complete:
    mov sp, bp
    pop bp
    retf
empty_file:
    xor dx, dx
    jmp short load_complete
_file_load_binary_nofatal endp
_TEXT ends
end
