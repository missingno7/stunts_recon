; ASMDOC Module: RAM archive rebuild and packed-plane to indexed-pixel expansion.
; ASMDOC Owner asm_load_25fa2; original source asm/file_load_shape2d_expand.ASM.
; ASMDOC Routine inventory: _file_load_shape2d_expand
; ASMDOC All BP stack offsets below follow PUSH BP / MOV BP,SP; data spans are caller-owned.
; ASMDOC Pure caller-buffer transformation, despite existing PLATFORM(video) tags. No self-modification or indirect callback.
.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _file_get_shape2d:far
public _file_load_shape2d_expand
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file load shape2d expand.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_memchunkoff (word ptr 6); arg_memchunkseg (word ptr 8); arg_mempagesoff (word ptr 10);
;   arg_mempagesseg (word ptr 12).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, DS, ES, SS; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_load_shape2d_expand
; ASMDOC purpose: Construct an archive of 8-times-wider indexed bitmap shapes from up to four packed 1-bit planes.
; ASMDOC convention: FAR caller-clean stack; BP offsets after PUSH BP / MOV BP,SP (entry SP offsets are BP offsets minus 2); RETF via shared
; ASMDOC body where applicable.
; ASMDOC inputs: BP+6 source archive offset; +8 segment; BP+10 destination archive offset; +12 segment; DF must already be clear;
; ASMDOC source/destination buffers paragraph aligned for _file_get_shape2d; source and destination spans must not destructively overlap;
; ASMDOC Source dimensions and low-word width*height in 1..8000 for pixel initialization/expansion; output 8*length must fit current segment
; ASMDOC offsets
; ASMDOC outputs: Copies source count/names to destination, constructs dword relative offsets and final archive size; Copies first 12
; ASMDOC shape-header bytes, multiplies destination width by 8, fills background with source byte[13]>>4 then ORs low-nibble plane colors for
; ASMDOC set bits MSB-first; Destination header bytes 12..15 are left untouched; no defined return value; Out-of-range signed length skips
; ASMDOC pixel fill/planes after headers/offsets have already been written
; ASMDOC preserves: BP DS SI DI SP; DF unchanged
; ASMDOC clobbers: AX BX CX DX ES arithmetic flags; DS transiently source/SS
; ASMDOC globals_read: none directly; see transitive effects stated in services
; ASMDOC globals_written: none directly; see transitive effects stated in services
; ASMDOC services: _file_get_shape2d called for each source and destination shape; its BX CX DX AX ES clobbers and paragraph-base assumption
; ASMDOC apply; No DOS/BIOS/video service, no allocation or resize; all storage supplied by caller
; ASMDOC dead: False
; ASMDOC Public name from existing plan: _file_load_shape2d_expand (unchanged).
_file_load_shape2d_expand proc far
    var_dstshapeoff = word ptr -16
    var_length = word ptr -14
    var_srcshapeoff = word ptr -12
    var_srcshapeseg = word ptr -10
    var_srcdataoff = word ptr -8
    var_offsets = word ptr -6
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
    sub     sp, 10h
    push    di
    push    si
    push    ds
    mov     ds, [bp+arg_memchunkseg]
    mov     si, [bp+arg_memchunkoff]
    mov     es, [bp+arg_mempagesseg]
    mov     di, [bp+arg_mempagesoff]
; ASMDOC Copy count and four-byte names only; destination offset table is rebuilt from zero.
    mov     cx, [si+4]
    mov     [bp+var_shapecount], cx
    shl     cx, 1
    add     cx, 1
    add     si, 4
    add     di, 4
    rep movsw
    xor     ax, ax
expand_store_output_value:
    mov     [bp+var_shapecounter], ax
    mov     [bp+var_offsets], di
    mov     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    mov     es:[di+2], ax ;/* PLATFORM(video): accesses display memory through ES. */
shape_expand_wait_loop:
    mov     ax, [bp+var_shapecounter]
    cmp     ax, [bp+var_shapecount]
    jl      short shape_expand_when_less
    pop     ds
    pop     si
    pop     di
    mov     sp, bp
    pop     bp
    retf
shape_expand_when_less:
    push    ax
shape_expand_continue_path:
    push    [bp+arg_memchunkseg]
shape_expand_continue_path_2:
    push    [bp+arg_memchunkoff]
    mov     ax, ss
    mov     ds, ax
    call    _file_get_shape2d
    add     sp, 6
    mov     ds, dx
    mov     si, ax
    mov     [bp+var_srcshapeseg], dx
    mov     [bp+var_srcshapeoff], ax
    mov     es, [bp+arg_mempagesseg]
    mov     di, [bp+var_offsets]
    mov     bx, es:[di] ;/* PLATFORM(video): accesses display memory through ES. */
    mov     cx, es:[di+2] ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, 4
    mov     [bp+var_offsets], di
; ASMDOC Length is low word of unsigned MUL; subsequent size shifts do not rotate carry into DX.
    mov     ax, [si]
    mul     word ptr [si+2]
    mov     [bp+var_length], ax
    shl     ax, 1
    shl     ax, 1
    shl     ax, 1
    add     ax, 10h
    add     ax, bx
    adc     dx, cx
    mov     bx, [bp+var_shapecounter]
    inc     bx
    cmp     bx, [bp+var_shapecount]
    jge     short expand_when_greater_or_equal
    mov     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    mov     es:[di+2], dx ;/* PLATFORM(video): accesses display memory through ES. */
    jmp     short shape_expand_when_condition
    ; align 2
    db 144
expand_when_greater_or_equal:
    mov     bx, [bp+var_shapecount]
    shl     bx, 1
    shl     bx, 1
    shl     bx, 1
    add     bx, 6
    add     ax, bx
    adc     dx, 0
    mov     di, [bp+arg_mempagesoff]
    mov     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    mov     es:[di+2], dx ;/* PLATFORM(video): accesses display memory through ES. */
shape_expand_when_condition:
    push    [bp+var_shapecounter]
    push    [bp+arg_mempagesseg]
    push    [bp+arg_mempagesoff]
    mov     ax, ss
    mov     ds, ax
    call    _file_get_shape2d
shape_expand_continue_path_3:
    add     sp, 6
shape_expand_continue_path_4:
    mov     es, dx
    mov     di, ax
    mov     [bp+var_dstshapeoff], ax
; ASMDOC Only six words of the header are copied; source bytes 12..15 become plane controls and are not copied.
    mov     ds, [bp+var_srcshapeseg]
    mov     cx, 6
    rep movsw
    mov     si, [bp+var_srcshapeoff]
shape_expand_load_source_value:
    mov     di, [bp+var_dstshapeoff]
    mov     ax, [si]
    shl     ax, 1
    shl     ax, 1
    shl     ax, 1
    mov     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    mov     al, [si+0Dh]
shape_expand_shift_value:
    shr     al, 1
    shr     al, 1
    shr     al, 1
    shr     al, 1
    mov     ah, al
    add     di, 10h
expand_load_source_value_2:
; ASMDOC Signed 1..8000 guard is after header/directory writes; invalid dimensions skip pixel work.
    mov     cx, [bp+var_length]
    cmp     cx, 0
    jle     short expand_when_less_or_equal
    cmp     cx, 1F40h
    jg      short expand_when_less_or_equal
    shl     cx, 1
    shl     cx, 1
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
expand_advance_source_pointer:
    add     si, 10h
    mov     [bp+var_srcdataoff], si
    xor     bx, bx
; ASMDOC Stop at first zero low-nibble color; expand each packed byte MSB-first and OR color into eight indexed pixels.
shape_expand_scan_loop:
    mov     si, [bp+var_srcshapeoff]
    mov     ah, [bx+si+0Ch]
    and     ah, 0Fh
    jz      short expand_when_less_or_equal
    mov     di, [bp+var_dstshapeoff]
    add     di, 10h
    mov     si, [bp+var_srcdataoff]
expand_load_source_value_3:
    mov     dx, [bp+var_length]
shape_expand_read_loop:
    lodsb
    mov     cx, 8
shape_expand_scan_loop_2:
    shl     al, 1
    jnb     short shape_expand_when_not_below
    or      es:[di], ah ;/* PLATFORM(video): accesses display memory through ES. */
shape_expand_when_not_below:
    inc     di
    loop    shape_expand_scan_loop_2
    dec     dx
    jg      short shape_expand_read_loop
    mov     [bp+var_srcdataoff], si
    shl     ah, 1
    inc     bx
    cmp     bx, 4
    jl      short shape_expand_scan_loop
expand_when_less_or_equal:
    inc     [bp+var_shapecounter]
    jmp     shape_expand_wait_loop
    ; align 2
    db 0
_file_load_shape2d_expand endp
S012_TEXT ends
end
