; ASMDOC Module: PES selected-plane transpose through caller scratch RAM.
; ASMDOC Owner load_260f6; original source asm/file_unflip_shape2d_pes.ASM.
; ASMDOC Routine inventory: _file_unflip_shape2d_pes
; ASMDOC All BP stack offsets below follow PUSH BP / MOV BP,SP; data spans are caller-owned.
; ASMDOC Pure memory transpose. No self-modification, code-data overlap, or indirect callback.
.8086
INCLUDE include\hw.inc
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _shapepointer:far
public _file_unflip_shape2d_pes
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file unflip shape2d pes.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_memchunkoff (word ptr
; 6); arg_memchunkseg (word ptr 8); arg_mempagesoff (word ptr 10);
;   arg_mempagesseg (word ptr 12).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, DS, ES; balanced explicit saves/restores observed: DI,
; DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_unflip_shape2d_pes
; ASMDOC purpose: Transpose each selected PES plane through scratch, copying it back to the same archive plane.
; ASMDOC convention: FAR caller-clean stack; BP offsets after PUSH BP / MOV BP,SP (entry SP offsets are BP
; offsets minus 2); RETF via shared
; ASMDOC body where applicable.
; ASMDOC inputs: BP+6 archive offset; +8 segment; BP+10 scratch offset; +12 segment; DF must already be clear;
; helper requires
; ASMDOC paragraph-aligned archive; scratch must fit width*height low-word bytes and not overlap active plane;
; Requires nonempty archive and
; ASMDOC nonzero dimensions for normal finite loops; source plane layout contains four consecutive width*height spans
; ASMDOC outputs: Selected plane byte[x*height+y] becomes plane byte[y*width+x]; advances every plane including
; unselected; Suppresses a
; ASMDOC whole shape if header[15]&0xF0; selects planes by bits 0..3 of header[14]>>4; Does not clear
; flip/suppression flags or change
; ASMDOC dimensions; no defined return value
; ASMDOC preserves: BP DS SI DI SP; DF unchanged
; ASMDOC clobbers: AX BX CX DX ES arithmetic flags; DS transiently archive/scratch
; ASMDOC globals_read: none directly; see transitive effects stated in services
; ASMDOC globals_written: none directly; see transitive effects stated in services
; ASMDOC services: _file_get_shape2d(archive,index) normalized return and AX BX CX DX ES clobbers apply; No
; video/DOS/BIOS service; scratch
; ASMDOC and archive RAM only
; ASMDOC dead: False
; ASMDOC Public name from existing plan: _file_unflip_shape2d_pes (unchanged).
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
; ASMDOC Do-while shape iteration: count zero is not checked before first lookup.
shape_unflip_wait_loop:
    push    [bp+var_shapecounter]
    push    [bp+arg_memchunkseg]
    push    [bp+arg_memchunkoff]
    mov     ax, seg dseg
    mov     ds, ax
    call    _shapepointer
    add     sp, 6
    mov     [bp+var_shapeofs], ax
    mov     [bp+var_shapeseg], dx
    mov     si, ax
    mov     ds, dx
; ASMDOC Suppression upper nibble overrides all four per-plane transpose bits.
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
; ASMDOC Consume one selection bit each iteration; unselected plane still advances by low-word width*height.
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
; ASMDOC Gather source by height stride into scratch in row-major order; zero width/height are not guarded.
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
; ASMDOC Copy the gathered plane back in-place; headers and flip flags remain untouched.
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

