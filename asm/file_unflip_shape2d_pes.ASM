.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _file_get_shape2d:far
public _file_unflip_shape2d_pes
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file unflip shape2d pes.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_memchunkoff (word ptr 6); arg_memchunkseg (word ptr 8); arg_mempagesoff (word ptr 10);
;   arg_mempagesseg (word ptr 12).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
_file_unflip_shape2d_pes proc far
    var_val = byte ptr -20
    var_iterations = byte ptr -18
    var_dataptr = word ptr -16
    var_length = word ptr -14
    var_height = word ptr -12
    var_width = word ptr -10
    var_shapeseg = word ptr -8
    var_shapeofs = word ptr -6
    var_shapecounter = word ptr -4
    var_shapecount = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_memchunkoff = word ptr 6
    arg_memchunkseg = word ptr 8
    arg_mempagesoff = word ptr 10
    arg_mempagesseg = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 14h
    push    ds
    push    si
    push    di
    mov     ax, [bp+arg_memchunkseg]
    mov     ds, ax
    mov     ax, [bp+arg_memchunkoff]
    mov     si, ax
    mov     bx, [si+4]
    mov     [bp+var_shapecount], bx
    mov     [bp+var_shapecounter], 0
shape_unflip_wait_loop:
    push    [bp+var_shapecounter]
    push    [bp+arg_memchunkseg]
    push    [bp+arg_memchunkoff]
    mov     ax, seg dseg
    mov     ds, ax
    call    _file_get_shape2d
    add     sp, 6
    mov     [bp+var_shapeofs], ax
    mov     [bp+var_shapeseg], dx
    mov     si, ax
    mov     ds, dx
    mov     al, [si+0Fh]
shape_unflip_mask_value:
    and     al, 0F0h
    jnz     short shape_unflip_wait_loop_2
    mov     bl, [si+0Eh]
    shr     bl, 1
    shr     bl, 1
    shr     bl, 1
    shr     bl, 1
    and     bl, 0Fh
    mov     [bp+var_val], bl
    jnz     short shape_unflip_when_nonzero
shape_unflip_wait_loop_2:
    inc     [bp+var_shapecounter]
    dec     [bp+var_shapecount]
    jg      short shape_unflip_wait_loop
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
shape_unflip_when_nonzero:
    mov     ax, [si+0]
    mov     [bp+var_width], ax
shape_unflip_load_source_value:
    mov     dx, [si+2]
    mov     [bp+var_height], dx
    mul     dx
    mov     [bp+var_length], ax
    add     si, 10h
unflip_store_output_value:
    mov     [bp+var_dataptr], si
unflip_store_output_value_2:
    mov     [bp+var_iterations], 4
shape_unflip_scan_loop:
    shr     [bp+var_val], 1
    jnb     short shape_unflip_when_not_below
unflip_load_source_value_2:
    mov     es, [bp+arg_mempagesseg]
    mov     di, [bp+arg_mempagesoff]
unflip_load_source_value_3:
    mov     ds, [bp+var_shapeseg]
    xor     dx, dx
unflip_load_source_value_4:
    mov     bx, [bp+var_height]
shape_unflip_fill_loop:
    mov     si, [bp+var_dataptr]
    add     si, dx
    mov     cx, [bp+var_width]
shape_unflip_fill_loop_2:
    mov     al, [si]
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
unflip_advance_source_pointer:
    add     si, bx
shape_unflip_continue_path:
    loop    shape_unflip_fill_loop_2
    inc     dx
shape_unflip_check_dx:
    cmp     dx, bx
    jnz     short shape_unflip_fill_loop
unflip_load_source_value_5:
    mov     es, [bp+var_shapeseg]
unflip_load_source_value_6:
    mov     di, [bp+var_dataptr]
unflip_load_source_value_7:
    mov     ds, [bp+arg_mempagesseg]
unflip_load_source_value_8:
    mov     si, [bp+arg_mempagesoff]
unflip_load_source_value_9:
    mov     cx, [bp+var_length]
shape_unflip_copy_bytes:
    rep movsb
shape_unflip_when_not_below:
    mov     ax, [bp+var_length]
shape_unflip_continue_path_2:
    add     [bp+var_dataptr], ax
decrement_bp_var_iterations:
    dec     [bp+var_iterations]
shape_unflip_continue_path_3:
    jnz     short shape_unflip_scan_loop
shape_unflip_continue_path_4:
    jmp     short shape_unflip_wait_loop_2
_file_unflip_shape2d_pes endp
S012_TEXT ends
dseg segment byte public 'STUNTSD'
dseg ends
end

