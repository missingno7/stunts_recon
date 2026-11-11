; ASMDOC Module: In-place palette-index mapping across all bitmap shapes in a RAM archive.
; ASMDOC Owner asm_load_25f48; original source asm/file_load_shape2d_palmap_apply.ASM.
; ASMDOC Routine inventory: _file_load_shape2d_palmap_apply
; ASMDOC All BP stack offsets below follow PUSH BP / MOV BP,SP; data spans are caller-owned.
; ASMDOC No hardware service or self-modifying code; existing PLATFORM(video) annotations are false for this
; resource transform.
.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _file_get_shape2d:far
public _file_load_shape2d_palmap_apply
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file load shape2d palmap apply.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_memchunkoff (word ptr
; 6); arg_memchunkseg (word ptr 8); arg_palmap (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, ES; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_load_shape2d_palmap_apply
; ASMDOC purpose: Map each raw shape pixel in-place through caller near palette lookup bytes.
; ASMDOC convention: FAR caller-clean stack; BP offsets after PUSH BP / MOV BP,SP (entry SP offsets are BP
; offsets minus 2); RETF via shared
; ASMDOC body where applicable.
; ASMDOC inputs: BP+6 archive offset; BP+8 archive segment; BP+10 near palette map in DS; DF must already be
; clear; _file_get_shape2d
; ASMDOC requires paragraph-aligned archive; Archive count and every width*height low word should be positive
; signed-loop-compatible; each
; ASMDOC source pixel is an unchecked u8 map index (0..255)
; ASMDOC outputs: Archive pixels rewritten map[pixel]; no defined return value; Count is signed compared;
; width*height uses MUL low word;
; ASMDOC zero product still executes 65536 pixel iterations via LOOP
; ASMDOC preserves: BP SI DI DS SP; DF unchanged
; ASMDOC clobbers: AX BX CX DX ES arithmetic flags; helper clobbers included
; ASMDOC globals_read: none directly; see transitive effects stated in services
; ASMDOC globals_written: none directly; see transitive effects stated in services
; ASMDOC services: _file_get_shape2d(archive,index) returns normalized DX:AX; saves SI and leaves DS/DI
; unchanged; clobbers AX BX CX DX ES
; ASMDOC and flags; No video, DOS or BIOS service; ES points at caller archive RAM
; ASMDOC dead: False
; ASMDOC Public name from existing plan: _file_load_shape2d_palmap_apply (unchanged).
_file_load_shape2d_palmap_apply proc far
    shapecounter = word ptr -4
    var_shapecount = word ptr -2
    arg_memchunkoff = word ptr 6
    arg_memchunkseg = word ptr 8
    arg_palmap = word ptr 10

    push    bp
    mov     bp, sp
    sub     sp, 4
    push    di
    push    si
    mov     es, [bp+arg_memchunkseg]
    mov     di, [bp+arg_memchunkoff]
    mov     si, [bp+arg_palmap]
; ASMDOC ES is the caller archive, not video memory. Signed comparison below treats counts >=0x8000 as negative.
    mov     cx, es:[di+4] ;/* PLATFORM(video): accesses display memory through ES. */
    mov     [bp+var_shapecount], cx
    xor     ax, ax
    mov     [bp+shapecounter], ax
shape_palmap_wait_loop:
    mov     ax, [bp+shapecounter]
    cmp     ax, [bp+var_shapecount]
    jl      short shape_palmap_when_less
    pop     si
    pop     di
    mov     sp, bp
    pop     bp
    retf
shape_palmap_when_less:
    push    ax
    push    [bp+arg_memchunkseg]
    push    [bp+arg_memchunkoff]
    call    _file_get_shape2d
    add     sp, 6
    mov     es, dx
    mov     di, ax
; ASMDOC Unsigned width*height MUL; only low 16 bits drive LOOP. No zero-product precheck.
    mov     ax, es:[di+0] ;/* PLATFORM(video): accesses display memory through ES. */
    mul     word ptr es:[di+2] ;/* PLATFORM(video): accesses display memory through ES. */
    mov     cx, ax
    add     di, 10h
    xor     bx, bx
shape_palmap_fill_loop:
; ASMDOC BL is full 8-bit pixel index; the map is not masked or bounded to the usual 16 initialized entries.
    mov     bl, es:[di] ;/* PLATFORM(video): accesses display memory through ES. */
    mov     al, [bx+si]
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    loop    shape_palmap_fill_loop
    inc     [bp+shapecounter]
    jmp     short shape_palmap_wait_loop
    db 0 ; retained zero fill at this boundary
_file_load_shape2d_palmap_apply endp
S012_TEXT ends
end
