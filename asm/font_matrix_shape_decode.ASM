; ASMDOC(font_shapes) Module font_matrix_shape_decode. Existing name describes the principal mixed contents;
; retain this full
; ASMDOC(font_shapes) module boundary, including patterned setup, status I/O and owned data. Public callable entries:
; ASMDOC(font_shapes) _font_op, _font_op2, _preRender_patterned, _nopsub_328C9, _nopsub_328DB, _mat_vec,
; _mat_multiply,
; ASMDOC(font_shapes) _mat_invert, _file_unflip_shape2d, _file_decomp_vle, _nopsub_32FEE, _video_get_status.
; Public DATA only:
; ASMDOC(font_shapes) _fontdefseg, _vector_op_base_z.
; ASMDOC(font_shapes) Contract offsets use BP after the standard prologue; FAR return IP/CS occupy BP+2/+4.
; Original source
; ASMDOC(font_shapes) comments remain verbatim and may contain older incomplete summaries; these added contracts
; describe
; ASMDOC(font_shapes) transitive shared exits.
.8086
INCLUDE include\platform_hw.inc
name asm012_141362
MATRIX struc
_11 dw ?
_21 dw ?
_31 dw ?
_12 dw ?
_22 dw ?
_32 dw ?
_13 dw ?
_23 dw ?
_33 dw ?
MATRIX ends
VECTOR struc
vx dw ?
vy dw ?
vz dw ?
VECTOR ends
SHAPE2D struc
s2d_width dw ?
s2d_height dw ?
s2d_unk1 dw ?
s2d_unk2 dw ?
s2d_pos_x dw ?
s2d_pos_y dw ?
s2d_unk3 db ?
s2d_unk4 db ?
s2d_unk5 db ?
s2d_unk6 db ?
SHAPE2D ends
S012_TEXT segment word public 'CODE'
S012_TEXT ends
extrn _fontdef_default:byte
_DATA segment word public 'DATA'
public _fontdefseg, _vector_op_base_z
; ASMDOC(font_shapes) Public DATA (not a callable entry): _fontdefseg. Writable DGROUP segment word selecting
; active font,
; ASMDOC(font_shapes) initialized with SEG _fontdef_default.
_fontdefseg dw seg _fontdef_default
; ASMDOC(font_shapes) Public DATA (not a callable entry): _vector_op_base_z. Writable DGROUP word initialized to
; 10; vector
; ASMDOC(font_shapes) near-plane depth used by external routines, no accesses in this owner.
_vector_op_base_z dw 10
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
dseg segment byte public 'STUNTSD'
dseg ends
assume cs:S012_TEXT, ds:DGROUP
extrn _line_pattern_bits:word
extrn _spritefunc:word
extrn _imagefunc:word
extrn _callbackflags2:byte
extrn _file_get_shape2d:far
extrn _draw_patterned_lines:far
extrn _preRender_line:far
extrn _prerender_shared_setup:near
public _font_op
public _font_op2
public _preRender_patterned
public _nopsub_328C9
public _nopsub_328DB
public _mat_vec
public _mat_multiply
public _mat_invert
public _file_unflip_shape2d
public _file_decomp_vle
public _nopsub_32FEE
public _video_get_status
S012_TEXT segment word public 'CODE'

; -----------------------------------------------------------------------------
; Purpose: Dispatch one font or glyph drawing operation.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_4 (word ptr 8).
; Register ABI: touches AX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _font_op (proposed _font_op): Measure text width modulo 65536 by summing advances
; of glyphs
; ASMDOC(font_shapes) whose offset is nonzero; no drawing.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] u16 near offset in DS to NUL-terminated legacy-byte text; [BP+8] u16
; maximum number of
; ASMDOC(font_shapes) present glyphs (not bytes); zero returns AX=0 without font access
; ASMDOC(font_shapes) outputs: AX = summed u16 glyph advances modulo 65536; missing glyphs neither add width nor
; decrement the
; ASMDOC(font_shapes) counter
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; SS; CS; SP (balanced); DF
; ASMDOC(font_shapes) clobbers: AX; BX; CL (CH unchanged); DX; ES; arithmetic flags
; ASMDOC(font_shapes) globals_read: _fontdefseg (DGROUP word: selected font segment); selected font ES:0010
; fixed advance,
; ASMDOC(font_shapes) ES:0014 proportional byte, ES:0016 + 2*unsigned text byte glyph-offset table, glyph width
; byte if
; ASMDOC(font_shapes) proportional
; ASMDOC(font_shapes) globals_written: none in this routine.
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) Shared epilogue lies in _font_op2; _font_op sets DX=count and jumps into its body.
; ASMDOC(font_shapes) _font_op2 initializes DX=0; shared DEC DX / JZ can stop after 65536 present glyphs even
; without NUL.
; ASMDOC(font_shapes) Font and text memory are ordinary data; ES glyph reads do not establish a hardware service.
_font_op proc far
    arg_maximum_characters = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     dx, [bp+arg_maximum_characters]
    or      dx, dx
    jnz     short font_op2_branch_path_01
    xor     ax, ax
    jmp     short font_op2_branch_path_03
    nop ; retained code pad at an even offset; EVEN would emit no byte
_font_op endp
; -----------------------------------------------------------------------------
; Purpose: Dispatch a font operation using the alternate glyph path.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BX, DX, SI, DI, BP, SP, ES; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _fontdefseg.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _font_op2 (proposed _font_op2): Measure text width modulo 65536 by summing
; advances of glyphs
; ASMDOC(font_shapes) whose offset is nonzero; no drawing.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] u16 near offset in DS to NUL-terminated legacy-byte text
; ASMDOC(font_shapes) outputs: AX = summed u16 glyph advances modulo 65536; missing glyphs neither add width nor
; decrement the
; ASMDOC(font_shapes) counter
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; SS; CS; SP (balanced); DF
; ASMDOC(font_shapes) clobbers: AX; BX; CL (CH unchanged); DX; ES; arithmetic flags
; ASMDOC(font_shapes) globals_read: _fontdefseg (DGROUP word: selected font segment); selected font ES:0010
; fixed advance,
; ASMDOC(font_shapes) ES:0014 proportional byte, ES:0016 + 2*unsigned text byte glyph-offset table, glyph width
; byte if
; ASMDOC(font_shapes) proportional
; ASMDOC(font_shapes) globals_written: none in this routine.
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) Shared epilogue lies in _font_op2; _font_op sets DX=count and jumps into its body.
; ASMDOC(font_shapes) _font_op2 initializes DX=0; shared DEC DX / JZ can stop after 65536 present glyphs even
; without NUL.
; ASMDOC(font_shapes) Font and text memory are ordinary data; ES glyph reads do not establish a hardware service.
_font_op2 proc far
    arg_text_offset = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    xor     dx, dx
; ASMDOC(font_shapes) Shared width-measurement core: font glyph lookup adds advances only; no destination is written.
font_op2_branch_path_01:
    mov     es, _fontdefseg
    mov     si, [bp+arg_text_offset]
    xor     ax, ax
    mov     cl, es:14h
    mov     di, es:10h
font_op2_loop_head_01:
    mov     bl, [si]
    or      bl, bl
    jz      short font_op2_branch_path_03
    inc     si
    xor     bh, bh
    shl     bx, 1
    mov     bx, es:[bx+16h] ;/* PLATFORM(video): accesses display memory through ES. */
    or      bx, bx
    jz      short font_op2_loop_head_01
    or      cl, cl
    jz      short font_op2_branch_path_02
    mov     bl, es:[bx] ;/* PLATFORM(video): accesses display memory through ES. */
    xor     bh, bh
    mov     di, bx
font_op2_branch_path_02:
    add     ax, di
    dec     dx
    jz      short font_op2_branch_path_03
    jmp     short font_op2_loop_head_01
font_op2_branch_path_03:
    pop     di
    pop     si
    pop     bp
    retf
_font_op2 endp
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender patterned.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 8);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _imagefunc, _line_pattern_bits, _spritefunc.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _preRender_patterned (proposed _preRender_patterned): Install patterned polygon
; callbacks,
; ASMDOC(font_shapes) reshape arguments, then tail-enter common polygon setup.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] u16 line pattern bits; [BP+8] u16 fill/color value; [BP+10] u16 vertex
; count; [BP+12] u16
; ASMDOC(font_shapes) near offset in DS to signed-word POINT2D pairs x,y
; ASMDOC(font_shapes) outputs: No scalar return contract; rasterization through _prerender_shared_setup; [BP-10]
; byte mode = 1
; ASMDOC(font_shapes) before shared setup
; ASMDOC(font_shapes) preserves: BP/SI/DI restored by shared renderer epilogue; SP balanced by shared renderer
; epilogue; DS
; ASMDOC(font_shapes) locally unchanged; callbacks must preserve any further required registers
; ASMDOC(font_shapes) clobbers: AX; BX/CX/DX/ES and flags through shared renderer; additional callback clobbers
; depend on
; ASMDOC(font_shapes) retained far segment halves
; ASMDOC(font_shapes) globals_read: _sprite1 clip/window fields through _prerender_shared_setup; remaining
; segment halves of
; ASMDOC(font_shapes) _spritefunc/_imagefunc far callback slots are retained, not initialized here;
; renderer/callback global
; ASMDOC(font_shapes) effects outside this module are not fully enumerated
; ASMDOC(font_shapes) globals_written: _line_pattern_bits = original [BP+6]; _spritefunc low offset word = OFFSET
; ASMDOC(font_shapes) _draw_patterned_lines (segment half retained); _imagefunc low offset word = OFFSET
; _preRender_line
; ASMDOC(font_shapes) (segment half retained); caller [BP+6] overwritten with original [BP+8] color; [BP+8]
; overwritten with
; ASMDOC(font_shapes) original [BP+10] vertex count; shared renderer scratch and destination pixels via tail
; target/callbacks;
; ASMDOC(font_shapes) further indirect effects depend on callback segment halves
; ASMDOC(font_shapes) services: _prerender_shared_setup near tail jump (requires this routine's 2000-byte local
; frame);
; ASMDOC(font_shapes) indirect far _imagefunc/_spritefunc renderer callbacks after setup
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) No RETF in the local stub; shared target owns all exits.
; ASMDOC(font_shapes) The unlabelled prologue after the first jump in _preRender_patterned uses mode 0 with
; pointer input, but
; ASMDOC(font_shapes) is not a public export.
_preRender_patterned proc far
    local_merge_mode = byte ptr -10
    arg_pattern = word ptr 6
    arg_color = word ptr 8
    arg_vertex_count = word ptr 10
    arg_vertices = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     si, [bp+arg_vertices]
    mov     [bp+local_merge_mode], 1
    jmp     short patterned_loop_head_01
; ASMDOC(font_shapes) Unlabelled alternate FAR-style prologue: same stack parameters as _preRender_patterned,
; but mode=0; the
; ASMDOC(font_shapes) preceding unconditional jump bypasses it. No PUBLIC names this address.
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     si, [bp+arg_vertices]
    mov     [bp+local_merge_mode], 0
; ASMDOC(font_shapes) Both pointer/inline-list patterned entry families join here; rewrite only
; pattern/color/count argument
; ASMDOC(font_shapes) slots, then transfer frame ownership to the shared renderer.
patterned_loop_head_01:
    mov     ax, [bp+arg_pattern]
    mov     _line_pattern_bits, ax
    mov     ax, [bp+arg_color]
    mov     [bp+arg_pattern], ax
    mov     ax, [bp+arg_vertex_count]
    mov     [bp+arg_color], ax
    mov     ax, offset _draw_patterned_lines
    mov     _spritefunc, ax
    mov     ax, offset _preRender_line
    mov     _imagefunc, ax
    jmp     _prerender_shared_setup
_preRender_patterned endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 328C9.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_6 (word ptr 12).
; Register ABI: touches SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _nopsub_328C9 (proposed _sprite_pattern_poly_args): Install patterned polygon
; callbacks,
; ASMDOC(font_shapes) reshape arguments, then tail-enter common polygon setup.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] u16 line pattern bits; [BP+8] u16 fill/color value; [BP+10] u16 vertex
; count; [BP+12...]
; ASMDOC(font_shapes) inline signed-word POINT2D pairs x,y; SI=BP+12, dereferenced through DS, therefore DS must
; address the
; ASMDOC(font_shapes) stack list
; ASMDOC(font_shapes) outputs: No scalar return contract; rasterization through _prerender_shared_setup; [BP-10]
; byte mode = 1
; ASMDOC(font_shapes) before shared setup
; ASMDOC(font_shapes) preserves: BP/SI/DI restored by shared renderer epilogue; SP balanced by shared renderer
; epilogue; DS
; ASMDOC(font_shapes) locally unchanged; callbacks must preserve any further required registers
; ASMDOC(font_shapes) clobbers: AX; BX/CX/DX/ES and flags through shared renderer; additional callback clobbers
; depend on
; ASMDOC(font_shapes) retained far segment halves
; ASMDOC(font_shapes) globals_read: _sprite1 clip/window fields through _prerender_shared_setup; remaining
; segment halves of
; ASMDOC(font_shapes) _spritefunc/_imagefunc far callback slots are retained, not initialized here;
; renderer/callback global
; ASMDOC(font_shapes) effects outside this module are not fully enumerated
; ASMDOC(font_shapes) globals_written: _line_pattern_bits = original [BP+6]; _spritefunc low offset word = OFFSET
; ASMDOC(font_shapes) _draw_patterned_lines (segment half retained); _imagefunc low offset word = OFFSET
; _preRender_line
; ASMDOC(font_shapes) (segment half retained); caller [BP+6] overwritten with original [BP+8] color; [BP+8]
; overwritten with
; ASMDOC(font_shapes) original [BP+10] vertex count; shared renderer scratch and destination pixels via tail
; target/callbacks;
; ASMDOC(font_shapes) further indirect effects depend on callback segment halves
; ASMDOC(font_shapes) services: _prerender_shared_setup near tail jump (requires this routine's 2000-byte local
; frame);
; ASMDOC(font_shapes) indirect far _imagefunc/_spritefunc renderer callbacks after setup
; ASMDOC(font_shapes) Reachability: No executable token reference or address-taking found in accepted asm/ and
; src/ outside
; ASMDOC(font_shapes) declaration/definition; agrees with existing rename plan. No indirect table reference
; found. Static
; ASMDOC(font_shapes) absence is scoped to accepted sources, not proof about arbitrary external callers.
; ASMDOC(font_shapes) No RETF in the local stub; shared target owns all exits.
; ASMDOC(font_shapes) The unlabelled prologue after the first jump in _preRender_patterned uses mode 0 with
; pointer input, but
; ASMDOC(font_shapes) is not a public export.
_nopsub_328C9 proc far
    local_merge_mode = byte ptr -10
    arg_inline_vertices = byte ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    lea     si, [bp+arg_inline_vertices]
    mov     [bp+local_merge_mode], 1
    jmp     short patterned_loop_head_01
_nopsub_328C9 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 328DB.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_6 (word ptr 12).
; Register ABI: touches SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _nopsub_328DB (proposed _sprite_pattern_poly_args_alt): Install patterned polygon
; callbacks,
; ASMDOC(font_shapes) reshape arguments, then tail-enter common polygon setup.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] u16 line pattern bits; [BP+8] u16 fill/color value; [BP+10] u16 vertex
; count; [BP+12...]
; ASMDOC(font_shapes) inline signed-word POINT2D pairs x,y; SI=BP+12, dereferenced through DS, therefore DS must
; address the
; ASMDOC(font_shapes) stack list
; ASMDOC(font_shapes) outputs: No scalar return contract; rasterization through _prerender_shared_setup; [BP-10]
; byte mode = 0
; ASMDOC(font_shapes) before shared setup
; ASMDOC(font_shapes) preserves: BP/SI/DI restored by shared renderer epilogue; SP balanced by shared renderer
; epilogue; DS
; ASMDOC(font_shapes) locally unchanged; callbacks must preserve any further required registers
; ASMDOC(font_shapes) clobbers: AX; BX/CX/DX/ES and flags through shared renderer; additional callback clobbers
; depend on
; ASMDOC(font_shapes) retained far segment halves
; ASMDOC(font_shapes) globals_read: _sprite1 clip/window fields through _prerender_shared_setup; remaining
; segment halves of
; ASMDOC(font_shapes) _spritefunc/_imagefunc far callback slots are retained, not initialized here;
; renderer/callback global
; ASMDOC(font_shapes) effects outside this module are not fully enumerated
; ASMDOC(font_shapes) globals_written: _line_pattern_bits = original [BP+6]; _spritefunc low offset word = OFFSET
; ASMDOC(font_shapes) _draw_patterned_lines (segment half retained); _imagefunc low offset word = OFFSET
; _preRender_line
; ASMDOC(font_shapes) (segment half retained); caller [BP+6] overwritten with original [BP+8] color; [BP+8]
; overwritten with
; ASMDOC(font_shapes) original [BP+10] vertex count; shared renderer scratch and destination pixels via tail
; target/callbacks;
; ASMDOC(font_shapes) further indirect effects depend on callback segment halves
; ASMDOC(font_shapes) services: _prerender_shared_setup near tail jump (requires this routine's 2000-byte local
; frame);
; ASMDOC(font_shapes) indirect far _imagefunc/_spritefunc renderer callbacks after setup
; ASMDOC(font_shapes) Reachability: No executable token reference or address-taking found in accepted asm/ and
; src/ outside
; ASMDOC(font_shapes) declaration/definition; agrees with existing rename plan. No indirect table reference
; found. Static
; ASMDOC(font_shapes) absence is scoped to accepted sources, not proof about arbitrary external callers.
; ASMDOC(font_shapes) No RETF in the local stub; shared target owns all exits.
; ASMDOC(font_shapes) The unlabelled prologue after the first jump in _preRender_patterned uses mode 0 with
; pointer input, but
; ASMDOC(font_shapes) is not a public export.
_nopsub_328DB proc far
    local_merge_mode = byte ptr -10
    arg_inline_vertices = byte ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    lea     si, [bp+arg_inline_vertices]
    mov     [bp+local_merge_mode], 0
    jmp     short patterned_loop_head_01
    db 0 ; retained zero fill at this boundary
_nopsub_328DB endp
; -----------------------------------------------------------------------------
; Purpose: Multiply a matrix by a vector using the supplied fixed-point values.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_inpVector (word ptr 6);
; arg_matrix (word ptr 8); arg_outVector (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _mat_vec (proposed _mat_vec): Multiply a column-major Q14 matrix by a signed-word
; vector, with
; ASMDOC(font_shapes) original ordered partial stores.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] DS near offset to 6-byte signed VECTOR x,y,z; [BP+8] DS near offset to
; 18-byte signed
; ASMDOC(font_shapes) MATRIX; [BP+10] DS near offset to writable 6-byte output VECTOR
; ASMDOC(font_shapes) outputs: DS:output receives x, then y, then z; each component stores first product then
; adds remaining
; ASMDOC(font_shapes) nonzero terms modulo 65536
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; ES; SS; CS; SP (balanced); DF
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; arithmetic flags
; ASMDOC(font_shapes) globals_read: none in this routine.
; ASMDOC(font_shapes) globals_written: DS:output[0..5]
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) Each signed 16x16 product is shifted left twice in DX:AX and its high word taken (product
; >> 14 modulo
; ASMDOC(font_shapes) word); partial results are individually truncated, then word-added. Column-major 18-byte MATRIX:
; ASMDOC(font_shapes) _11,_21,_31,_12,_22,_32,_13,_23,_33.
; ASMDOC(font_shapes) Input/output may alias: the routine rereads components and matrix cells after earlier
; output writes; a
; ASMDOC(font_shapes) temporary-copy mathematical multiplication changes such behavior.
_mat_vec proc far
    arg_inpVector = word ptr 6
    arg_matrix = word ptr 8
    arg_outVector = word ptr 10

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     bx, [bp+arg_matrix]
    mov     si, [bp+arg_inpVector]
    mov     di, [bp+arg_outVector]
    xor     dx, dx
    mov     ax, [bx+MATRIX._11]
    or      ax, ax
    jz      short matrix_vec_branch_path_01
    mov     cx, [si+VECTOR.vx]
    or      cx, cx
    jz      short matrix_vec_branch_path_01
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
; ASMDOC(font_shapes) Store the first truncated product immediately; later source reads can observe aliased
; output writes.
matrix_vec_branch_path_01:
    mov     [di+VECTOR.vx], dx
    mov     ax, [bx+MATRIX._12]
    or      ax, ax
    jz      short matrix_vec_branch_path_02
    mov     cx, [si+VECTOR.vy]
    or      cx, cx
    jz      short matrix_vec_branch_path_02
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vx], dx
matrix_vec_branch_path_02:
    mov     ax, [bx+MATRIX._13]
    or      ax, ax
    jz      short matrix_vec_branch_path_03
    mov     cx, [si+VECTOR.vz]
    or      cx, cx
    jz      short matrix_vec_branch_path_03
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vx], dx
matrix_vec_branch_path_03:
    xor     dx, dx
    mov     ax, [bx+MATRIX._21]
    or      ax, ax
    jz      short matrix_vec_branch_path_04
    mov     cx, [si+VECTOR.vx]
    or      cx, cx
    jz      short matrix_vec_branch_path_04
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
matrix_vec_branch_path_04:
    mov     [di+VECTOR.vy], dx
    mov     ax, [bx+MATRIX._22]
    or      ax, ax
    jz      short matrix_vec_branch_path_05
    mov     cx, [si+VECTOR.vy]
    or      cx, cx
    jz      short matrix_vec_branch_path_05
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+2], dx
matrix_vec_branch_path_05:
    mov     ax, [bx+MATRIX._23]
    or      ax, ax
    jz      short matrix_vec_branch_path_06
    mov     cx, [si+VECTOR.vz]
    or      cx, cx
    jz      short matrix_vec_branch_path_06
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vy], dx
matrix_vec_branch_path_06:
    xor     dx, dx
    mov     ax, [bx+MATRIX._31]
    or      ax, ax
    jz      short matrix_vec_branch_path_07
    mov     cx, [si+VECTOR.vx]
    or      cx, cx
    jz      short matrix_vec_branch_path_07
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
matrix_vec_branch_path_07:
    mov     [di+VECTOR.vz], dx
    mov     ax, [bx+MATRIX._32]
    or      ax, ax
    jz      short matrix_vec_branch_path_08
    mov     cx, [si+VECTOR.vy]
    or      cx, cx
    jz      short matrix_vec_branch_path_08
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vz], dx
matrix_vec_branch_path_08:
    mov     ax, [bx+MATRIX._33]
    or      ax, ax
    jz      short matrix_vec_branch_path_09
    mov     cx, [si+VECTOR.vz]
    or      cx, cx
    jz      short matrix_vec_branch_path_09
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vz], dx
matrix_vec_branch_path_09:
    pop     di
    pop     si
    pop     bp
    retf
_mat_vec endp
; -----------------------------------------------------------------------------
; Purpose: Multiply two fixed-point matrices.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_leftMatrix (word ptr 8);
; arg_outputMatrix (word ptr 10); arg_rightMatrix (word ptr 6).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _mat_multiply (proposed _mat_multiply): Multiply two signed-word Q14 3x3 matrices in
; ASMDOC(font_shapes) column-major storage, retaining operand/output alias order.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] DS near offset to right MATRIX (18 bytes); [BP+8] DS near offset to left
; MATRIX (18
; ASMDOC(font_shapes) bytes); [BP+10] DS near offset to writable output MATRIX (18 bytes)
; ASMDOC(font_shapes) outputs: DS:output = left * right under the stored matrix convention; cells emitted in
; increasing word
; ASMDOC(font_shapes) address, each first term stored before remaining two are added
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; ES; SS; CS; SP (balanced); DF
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; arithmetic flags
; ASMDOC(font_shapes) globals_read: none in this routine.
; ASMDOC(font_shapes) globals_written: DS:output[0..17]
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) Each signed 16x16 product is shifted left twice in DX:AX and its high word taken (product
; >> 14 modulo
; ASMDOC(font_shapes) word); partial results are individually truncated, then word-added. Column-major 18-byte MATRIX:
; ASMDOC(font_shapes) _11,_21,_31,_12,_22,_32,_13,_23,_33.
; ASMDOC(font_shapes) Preserve instruction write order if any ranges overlap; no temporary matrix is made.
_mat_multiply proc far
    arg_rightMatrix = word ptr 6
    arg_leftMatrix = word ptr 8
    arg_outputMatrix = word ptr 10

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     si, [bp+arg_rightMatrix]
    mov     bx, [bp+arg_leftMatrix]
    mov     di, [bp+arg_outputMatrix]
    mov     cx, 9
matrix_mul_loop_head_01:
    xor     dx, dx
    mov     ax, [si+MATRIX._11]
    or      ax, ax
    jz      short matrix_mul_branch_path_01
    cmp     [bx+MATRIX._11], 0
    jz      short matrix_mul_branch_path_01
    imul    [bx+MATRIX._11]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
matrix_mul_branch_path_01:
    mov     [di+MATRIX._11], dx
    mov     ax, [si+MATRIX._21]
    or      ax, ax
    jz      short matrix_mul_branch_path_02
    cmp     [bx+MATRIX._12], 0
    jz      short matrix_mul_branch_path_02
    imul    [bx+MATRIX._12]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+MATRIX._11], dx
matrix_mul_branch_path_02:
    mov     ax, [si+MATRIX._31]
    or      ax, ax
    jz      short matrix_mul_branch_path_03
    cmp     [bx+MATRIX._13], 0
    jz      short matrix_mul_branch_path_03
    imul    [bx+MATRIX._13]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+MATRIX._11], dx
matrix_mul_branch_path_03:
    add     di, 2
    cmp     cx, 7
    jz      short matrix_mul_branch_path_04
    cmp     cx, 4
    jz      short matrix_mul_branch_path_04
    add     bx, 2
    loop    matrix_mul_loop_head_01
    pop     di
    pop     si
    pop     bp
    retf
matrix_mul_branch_path_04:
    sub     bx, 4
    add     si, 6
    loop    matrix_mul_loop_head_01
    pop     di
    pop     si
    pop     bp
    retf
_mat_multiply endp
; -----------------------------------------------------------------------------
; Purpose: Invert the fixed-point matrix in the supplied workspace.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_mat_in (word ptr 6);
; arg_mat_out (word ptr 8).
; Register ABI: touches AX, CX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _mat_invert (proposed _mat_invert): Transpose a 3x3 word matrix; only an
; orthonormal-matrix
; ASMDOC(font_shapes) inverse by interpretation.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] DS near offset to 18-byte input MATRIX; [BP+8] DS near offset to 18-byte
; output MATRIX
; ASMDOC(font_shapes) outputs: Equal offsets: swap off-diagonal pairs (2,6), (4,12), (10,14) in place. Distinct
; offsets: nine
; ASMDOC(font_shapes) transposed word copies in source instruction order.
; ASMDOC(font_shapes) preserves: BP; SI; DI; BX; DX; DS; ES; SS; CS; SP (balanced); DF
; ASMDOC(font_shapes) clobbers: AX; CX (=9); arithmetic flags
; ASMDOC(font_shapes) globals_read: none in this routine.
; ASMDOC(font_shapes) globals_written: DS:output[0..17] (or input for identical offsets)
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) No determinant, division, or general matrix inversion. Distinct partially overlapping
; ranges are not
; ASMDOC(font_shapes) protected.
; ASMDOC(font_shapes) The fliphandlers table after RETF belongs to the same preserved extent and is reached
; indirectly only by
; ASMDOC(font_shapes) _file_unflip_shape2d.
_mat_invert proc far
    arg_mat_in = word ptr 6
    arg_mat_out = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     si, [bp+arg_mat_in]
    mov     di, [bp+arg_mat_out]
    mov     cx, 9
    cmp     si, di
    jnz     short matrix_inv_branch_path_01
    mov     ax, [si+MATRIX._21]
    xchg    ax, [si+MATRIX._12]
    mov     [si+MATRIX._21], ax
    mov     ax, [si+MATRIX._31]
    xchg    ax, [si+MATRIX._13]
    mov     [si+MATRIX._31], ax
    mov     ax, [si+MATRIX._32]
    xchg    ax, [si+MATRIX._23]
    mov     [si+MATRIX._32], ax
    pop     di
    pop     si
    pop     bp
    retf
matrix_inv_branch_path_01:
    mov     ax, [si+MATRIX._11]
    mov     [di+MATRIX._11], ax
    mov     ax, [si+MATRIX._21]
    mov     [di+MATRIX._12], ax
    mov     ax, [si+MATRIX._31]
    mov     [di+MATRIX._13], ax
    mov     ax, [si+MATRIX._12]
    mov     [di+MATRIX._21], ax
    mov     ax, [si+MATRIX._22]
    mov     [di+MATRIX._22], ax
    mov     ax, [si+MATRIX._32]
    mov     [di+MATRIX._23], ax
    mov     ax, [si+MATRIX._13]
    mov     [di+MATRIX._31], ax
    mov     ax, [si+MATRIX._23]
    mov     [di+MATRIX._32], ax
    mov     ax, [si+MATRIX._33]
    mov     [di+MATRIX._33], ax
    pop     di
    pop     si
    pop     bp
    retf
    db 0 ; retained zero fill at this boundary
; ASMDOC(font_shapes) Code-resident three-entry near jump table used by the next routine, not executable
; fallthrough from
; ASMDOC(font_shapes) matrix transpose.
fliphandlers     dw offset fliptype0
    dw offset fliptype1
    dw offset fliptype2
_mat_invert endp
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file unflip shape2d.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_memchunkofs (word ptr
; 6); arg_memchunkseg (word ptr 8); arg_mempagesofs (word ptr 10);
;   arg_mempagesseg (word ptr 12).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed:
; DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _file_unflip_shape2d (proposed _file_unflip_shape2d): Transform flagged
; byte-per-pixel shapes
; ASMDOC(font_shapes) in an archive through a caller-provided scratch far buffer.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] archive far offset; [BP+8] archive far segment; [BP+10] scratch far offset;
; [BP+12]
; ASMDOC(font_shapes) scratch far segment; Archive word +4 is shape count; _file_get_shape2d resolves each shape
; far pointer;
; ASMDOC(font_shapes) Shapes use u16 width +0, height +2; flags +0E upper nibble selects mode, +0F upper nibble
; suppresses
; ASMDOC(font_shapes) transform; pixel bytes start +10h
; ASMDOC(font_shapes) outputs: AX=0 after loop completion; AX=1 immediately on an unsuppressed mode >=4, leaving
; later shapes
; ASMDOC(font_shapes) untouched; Transformed pixel payload is copied scratch->shape for low16(width*height) bytes;
; ASMDOC(font_shapes) flags/header not updated
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; CS; SS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared
; ASMDOC(font_shapes) globals_read: CS:fliphandlers three near code offsets; archive shape directory and payload
; ASMDOC(font_shapes) globals_written: scratch far buffer; transformed shape pixel payload; indirect
; _file_get_shape2d effects
; ASMDOC(font_shapes) not specified here
; ASMDOC(font_shapes) services: _file_get_shape2d far call with archive far pointer and index; DS temporarily
; set to SEG dseg;
; ASMDOC(font_shapes) CS:fliphandlers indexed near jump (modes 1,2,3)
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) No display service: STOSB/MOVSW targets caller scratch or shape memory.
; ASMDOC(font_shapes) Positive nonzero dimensions, usable offsets and sufficient scratch/source are caller
; obligations; loops
; ASMDOC(font_shapes) are entered before all zero-size checks. Archive count zero still enters one lookup.
; ASMDOC(font_shapes) Mode 1: out[y*w+x]=in[x*h+y]. Mode 2: even y use floor(y/2)+x*h; odd y use
; floor((h+y)/2)+x*h (odd-row
; ASMDOC(font_shapes) loop entered even for h=1). Mode 3 interleaves columns with ceil(h/2), floor(h/2) strides.
; ASMDOC(font_shapes) Mode 2 h=1 still emits an odd scratch row; only w*h bytes copy back. No segment-wrap
; adjustment exists
; ASMDOC(font_shapes) in these transform loops.
_file_unflip_shape2d proc far
    var_height = word ptr -14
    var_width = word ptr -12
    local_scratch_segment = word ptr -10
    local_scratch_offset = word ptr -8
    var_numshapes = word ptr -6
    var_counter = word ptr -4
    local_shape_count = word ptr -2
    arg_memchunkofs = word ptr 6
    arg_memchunkseg = word ptr 8
    arg_mempagesofs = word ptr 10
    arg_mempagesseg = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    cld
    mov     ax, [bp+arg_memchunkseg]
    mov     ds, ax
    mov     ax, [bp+arg_memchunkofs]
    mov     si, ax
    mov     ax, [bp+arg_mempagesseg]
    mov     [bp+local_scratch_segment], ax
    mov     ax, [bp+arg_mempagesofs]
    mov     [bp+local_scratch_offset], ax
    mov     bx, [si+4]      ; number of shapes in binary
    mov     [bp+local_shape_count], bx
    mov     [bp+var_counter], 0
unflip_shape_loop_head_01:
    push    [bp+var_counter]
    push    [bp+arg_memchunkseg]
    push    [bp+arg_memchunkofs]
    mov     ax, seg dseg
    mov     ds, ax
    call    _file_get_shape2d; get shape in binary at index
    add     sp, 6
    mov     [bp+var_numshapes], ax
    mov     si, ax
    mov     ds, dx
    mov     di, [bp+local_scratch_offset]
    mov     es, [bp+local_scratch_segment]
    mov     al, [si+SHAPE2D.s2d_unk6]; unk6 = is flipped
    and     al, 0F0h
    jnz     short unflip_shape_loop_head_02
    mov     al, [si+SHAPE2D.s2d_unk5]; (unk4 >> 4) = fliptype, 1..3
    shr     al, 1
    shr     al, 1
    shr     al, 1
    shr     al, 1
    jz      short unflip_shape_loop_head_02
    cmp     al, 4
    jb      short unflip_shape_branch_path_01
    mov     ax, 1
    jmp     short unflip_shape_join_01
    even ; word-align the following entry
unflip_shape_loop_head_02:
    inc     [bp+var_counter]
    dec     [bp+local_shape_count]
    jg      short unflip_shape_loop_head_01
    xor     ax, ax
unflip_shape_join_01:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
; ASMDOC(font_shapes) Unsuppressed mode 1..3 dispatches via fliphandlers; mode >=4 already returned AX=1.
unflip_shape_branch_path_01:
    mov     dx, [si+SHAPE2D.s2d_width]
    mov     [bp+var_width], dx
    mov     dx, [si+SHAPE2D.s2d_height]
    mov     [bp+var_height], dx
    xor     dx, dx
    xor     ah, ah
    mov     bx, ax
    dec     bx
    shl     bx, 1
    jmp     cs:fliphandlers[bx]; jump to flip type handler
; ASMDOC(font_shapes) Mode 1: transpose column-major pixels through scratch, using full height as source column
; stride.
fliptype0:
    mov     bx, si
    add     bx, 10h
    add     bx, dx
    mov     cx, [bp+var_width]
unflip_shape_loop_head_03:
    mov     al, [bx]
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     bx, [bp+var_height]
    loop    unflip_shape_loop_head_03
    inc     dx
    cmp     dx, [bp+var_height]
    jnz     short fliptype0
    jmp     unflip_shape_branch_path_02
; ASMDOC(font_shapes) Mode 2: emit alternating even/odd destination rows; both loops enter before checking
; height, including
; ASMDOC(font_shapes) h=1.
fliptype1:
    mov     bx, dx
    shr     bx, 1
    add     bx, 10h
    add     bx, si
    mov     cx, [bp+var_width]
unflip_shape_loop_head_04:
    mov     al, [bx]
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     bx, [bp+var_height]
    loop    unflip_shape_loop_head_04
    add     di, [bp+var_width]
    inc     dx
    inc     dx
    cmp     dx, [bp+var_height]
    jb      short fliptype1
    mov     dx, 1
    mov     di, [bp+local_scratch_offset]
    add     di, [bp+var_width]
unflip_shape_loop_head_05:
    mov     bx, dx
    add     bx, [bp+var_height]
    shr     bx, 1
    add     bx, 10h
    add     bx, si
    mov     cx, [bp+var_width]
unflip_shape_loop_head_06:
    mov     al, [bx]
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     bx, [bp+var_height]
    loop    unflip_shape_loop_head_06
    add     di, [bp+var_width]
    inc     dx
    inc     dx
    cmp     dx, [bp+var_height]
    jb      short unflip_shape_loop_head_05
    jmp     short unflip_shape_branch_path_02
    even ; word-align the following entry
; ASMDOC(font_shapes) Mode 3: alternating source half-column strides ceil(height/2),floor(height/2); this mode
; is implemented
; ASMDOC(font_shapes) here.
fliptype2:
    mov     bx, dx
    shr     bx, 1
    add     bx, 10h
    add     bx, [bp+var_numshapes]
    mov     cx, [bp+var_width]
    mov     si, [bp+var_height]
    shr     si, 1
    adc     si, 0
unflip_shape_loop_head_07:
    mov     al, [bx]
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     bx, si
    loop    unflip_shape_loop_head_07
    inc     dx
    cmp     dx, [bp+var_height]
    jz      short unflip_shape_branch_path_02
    mov     cx, [bp+var_width]
    mov     si, [bp+var_height]
    shr     si, 1
unflip_shape_loop_head_08:
    mov     al, [bx]
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     bx, si
    loop    unflip_shape_loop_head_08
    inc     dx
    cmp     dx, [bp+var_height]
    jnz     short fliptype2
; ASMDOC(font_shapes) Copy low16(width*height) bytes from scratch back over shape pixels; header flags are retained.
unflip_shape_branch_path_02:
    mov     di, [bp+var_numshapes]
    mov     ax, ds
    mov     es, ax
    add     di, 10h
    mov     si, [bp+local_scratch_offset]
    mov     ds, [bp+local_scratch_segment]
    mov     ax, [bp+var_width]
    mul     [bp+var_height]
    mov     cx, ax
    shr     cx, 1
    jb      short unflip_shape_branch_path_03
    rep movsw
    jmp     unflip_shape_loop_head_02
unflip_shape_branch_path_03:
    rep movsw
    movsb
    jmp     unflip_shape_loop_head_02
; ASMDOC(font_shapes) Mutable code-segment decoder data starts here: 16 word escape bases, then 16 word limits and 256
; ASMDOC(font_shapes) alphabet bytes in vle_esc2 storage. These bytes are not opcodes.
; Initialized storage: 16 word elements, each 0; verified all entries.
vle_esc1 label word
    REPT 16
    dw 0
    ENDM
; Initialized storage: 288 byte elements, each 0; verified all entries.
vle_esc2 label byte
    REPT 288
    db 00h
    ENDM
_file_unflip_shape2d endp
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file decomp vle.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_dstoff (word ptr 10);
; arg_dstseg (word ptr 12); arg_srcoff (word ptr 6); arg_srcseg
;   (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed:
; DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _callbackflags2.
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _file_decomp_vle (proposed _file_decomp_vle): Decode one variable-length
; entropy-coded pass,
; ASMDOC(font_shapes) including an additive branch, with mutable code-segment decode tables.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] compressed pass far offset; [BP+8] compressed pass far segment; [BP+10]
; destination far
; ASMDOC(font_shapes) offset; [BP+12] destination far segment; Source +1..3 = output length u24 little-endian;
; +4 = code-width
; ASMDOC(font_shapes) count (low 7 bits), additive flag (bit 7); following counts, alphabet then bitstream; Valid
; ASMDOC(font_shapes) code-width/count tables and enough source/destination storage required; decoder has no bounds or
; ASMDOC(font_shapes) malformed-stream error checks
; ASMDOC(font_shapes) outputs: DX:AX = original u24 declared output length; Destination bytes decoded in MSB-first
; ASMDOC(font_shapes) canonical-code order; high flag selects additive branch; Source/destination wrap
; increments segment by
; ASMDOC(font_shapes) 1000h when offset rolls to zero in the bitstream/output loops
; ASMDOC(font_shapes) preserves: BP; DI; CS; SS; SP balanced on ordinary completion; DS and SI only if symbol
; lookup slots
; ASMDOC(font_shapes) 252..255 are not populated/overwritten
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared; DS/SI when their saved stack
; slots are
; ASMDOC(font_shapes) overwritten by upper short-lookup symbols; other saved state can be corrupted by unchecked table
; ASMDOC(font_shapes) construction or additive SS writes
; ASMDOC(font_shapes) globals_read: CS:vle_esc1 16 words, CS:vle_esc2 first 16 words limits, CS:425Ch alphabet
; bytes after
; ASMDOC(font_shapes) table construction; _callbackflags2 used only as OFFSET anchor (_callbackflags2+72h), not
; read or called
; ASMDOC(font_shapes) globals_written: For well-formed widths/alphabet: CS:vle_esc1[0..31], CS:vle_esc2
; limit/alphabet area
; ASMDOC(font_shapes) [0..287]; unchecked malformed counts can write beyond these ranges; caller destination far
; buffer;
; ASMDOC(font_shapes) Additive long-code path also uses SS:[BP-14] after repurposing BP; see notes (not safely
; describable as
; ASMDOC(font_shapes) original stack-local access)
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Live indirect export: compression_type far-pointer table stores this
; address, and the
; ASMDOC(font_shapes) resource decompressor calls through that table.
; ASMDOC(font_shapes) For well-formed tables, code-segment writes are decode data-table mutation, not opcode
; patching. Routine
; ASMDOC(font_shapes) is non-reentrant, and recursive/concurrent decode is unsafe.
; ASMDOC(font_shapes) Ordinary decode: 512 local bytes form 256-byte symbol and length lookup arrays; lengths >8
; use escape
; ASMDOC(font_shapes) tables.
; ASMDOC(font_shapes) The additive long-code branch sets BP=000Eh, then accesses [BP+var_codoff] where
; var_codoff=-14, so
; ASMDOC(font_shapes) first store is SS:0000. BP is subsequently advanced or replaced by a symbol-table offset
; before later
; ASMDOC(font_shapes) accesses. Preserve this observed behavior; correctness on additive long-code inputs is not
; established.
; ASMDOC(font_shapes) Declared zero output length is not rejected before the first output. Initial two-byte bit
; lookahead and
; ASMDOC(font_shapes) count/alphabet setup do not normalize source segment at 64K crossings.
; ASMDOC(font_shapes) Literal stack layout: symbol bytes BP-272..BP-17 overlap saved DS at BP-20..BP-19 (indices
; 252,253) and
; ASMDOC(font_shapes) saved SI at BP-18..BP-17 (indices 254,255). Saved DI is BP-16..BP-15. Thus PUSH/POP
; symmetry alone does
; ASMDOC(font_shapes) not guarantee DS/SI for every table.
; ASMDOC(font_shapes) Additive flag is retained in var_codlen: the signed BH comparisons during short-lookup
; construction also
; ASMDOC(font_shapes) see that high bit. Behavior for additive inputs is not validated by this static documentation.
_file_decomp_vle proc far
    var_lengths = byte ptr -528
    var_symbols = byte ptr -272
    var_codoff = word ptr -14
    var_codlen = word ptr -10
    var_lenhi = word ptr -8
    var_lenlo = word ptr -6
    var_lenlefthi = word ptr -4
    var_lenleftlo = word ptr -2
    arg_srcoff = word ptr 6
    arg_srcseg = word ptr 8
    arg_dstoff = word ptr 10
    arg_dstseg = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    di
    push    si
    push    ds
    cld
    sub     sp, 200h
    mov     ds, [bp+arg_srcseg]
    mov     si, [bp+arg_srcoff]
    mov     ax, [si+1]
    mov     [bp+var_lenleftlo], ax
    mov     [bp+var_lenlo], ax
    mov     al, [si+3]
    xor     ah, ah
    mov     [bp+var_lenlefthi], ax
    mov     [bp+var_lenhi], ax
    mov     al, [si+4]
    xor     ah, ah
    mov     [bp+var_codlen], ax
    and     ax, 7Fh
    add     si, 5
    mov     [bp+var_codoff], si
    lea     cx, _callbackflags2+72h
    shl     ax, 1
    mov     di, ax
    xor     bx, bx
    xor     dx, dx
; ASMDOC(font_shapes) Build CS escape tables and copy the alphabet into CS:425Ch; this shared state makes the decoder
; ASMDOC(font_shapes) non-reentrant.
gen_esc:
    mov     cs:vle_esc1[bx], cx
    shl     dx, 1
    sub     cs:vle_esc1[bx], dx
    lodsb
    xor     ah, ah
    add     dx, ax
    add     cx, ax
    or      ax, ax
    jz      short $+2
    mov     word ptr cs:vle_esc2[bx], dx
    add     bx, 2
    cmp     bx, di
    jl      short gen_esc
    sub     cx, 425Ch
    mov     ax, cs
    mov     es, ax
    mov     di, 425Ch
    rep movsb
    push    si
    mov     bx, [bp+var_codlen]
    mov     bh, bl
    cmp     bh, 8
    jle     short vle_decode_branch_path_01
    mov     bh, 8
vle_decode_branch_path_01:
    xor     si, si
    mov     bl, 1
    mov     cl, 80h ; ' '
    mov     di, cs:vle_esc1
vle_decode_loop_head_01:
    xchg    si, [bp+var_codoff]
    lodsb
    xchg    si, [bp+var_codoff]
    mov     dh, al
    or      al, al
    jz      short vle_decode_branch_path_02
vle_decode_loop_head_02:
    mov     ch, cl
    mov     dl, cs:[di]
    inc     di
vle_decode_loop_head_03:
    mov     [bp+si+var_symbols], dl
    mov     [bp+si+var_lengths], bl
    inc     si
    dec     ch
    jnz     short vle_decode_loop_head_03
vle_decode_branch_path_02:
    dec     dh
    jg      short vle_decode_loop_head_02
    shr     cl, 1
    inc     bl
    cmp     bl, bh
    jle     short vle_decode_loop_head_01
; ASMDOC(font_shapes) Complete the 256-entry stack length lookup with >8-bit escape markers.
pad_lens:
    mov     [bp+si+var_lengths], 40h ; '@'
    inc     si
    cmp     si, 100h
    jnz     short pad_lens
    pop     si
    mov     bx, si
    mov     es, [bp+arg_dstseg]
    mov     di, [bp+arg_dstoff]
    mov     dl, [bx]
    inc     bx
    mov     dh, [bx]
    inc     bx
    mov     ah, 8
    test    [bp+var_codlen], 80h
    jz      short vle_decode_loop_head_04
    jmp     vle_decode_loop_head_12
vle_decode_loop_head_04:
    mov     si, dx
    and     si, 0FFh
    mov     cl, [bp+si+var_lengths]
    cmp     cl, 8
    ja      short len_gt_8bit
    mov     al, [bp+si+var_symbols]
    stosb
    cmp     cl, ah
    jle     short vle_decode_loop_head_05
    xchg    cl, ah
    rol     dx, cl
    xchg    cl, ah
    sub     cl, ah
    jmp     short vle_decode_join_01
    nop ; retained code pad at an even offset; EVEN would emit no byte
vle_decode_loop_head_05:
    rol     dx, cl
    sub     ah, cl
    or      di, di
    jz      short vle_decode_branch_path_03
vle_decode_loop_head_06:
    dec     [bp+var_lenleftlo]
    jnz     short vle_decode_loop_head_04
    dec     [bp+var_lenlefthi]
    jge     short vle_decode_loop_head_04
    jmp     vle_decode_loop_head_11
vle_decode_branch_path_03:
    mov     di, es
    add     di, 1000h
    mov     es, di
    xor     di, di
    jmp     short vle_decode_loop_head_06
vle_decode_join_01:
    mov     dh, [bx]
    inc     bx
    jz      short vle_decode_branch_path_04
vle_decode_loop_head_07:
    mov     ah, 8
    jmp     short vle_decode_loop_head_05
vle_decode_branch_path_04:
    mov     bx, ds
    add     bx, 1000h
    mov     ds, bx
    xor     bx, bx
    jmp     short vle_decode_loop_head_07
; ASMDOC(font_shapes) Long-code fallback saves the real frame BP, then uses BP as escape-table byte index and
; symbol address.
len_gt_8bit:
    push    si
    push    bp
    mov     si, bx
    mov     bp, 0Eh
    mov     bh, dh
    xor     dh, dh
    xor     ch, ch
vle_decode_loop_head_08:
    cmp     ah, 0
    jz      short vle_decode_branch_path_06
vle_decode_loop_head_09:
    cmp     ch, 0
    jz      short vle_decode_branch_path_05
    mov     dh, bh
    mov     bx, si
    pop     bp
    pop     si
    jmp     short vle_decode_loop_head_05
vle_decode_branch_path_05:
    shl     bh, 1
    rcl     dx, 1
    sub     ah, 1
    add     bp, 2
    cmp     dx, word ptr cs:vle_esc2[bp]
    jnb     short vle_decode_loop_head_08
    add     dx, cs:vle_esc1[bp]
    mov     bp, dx
    mov     al, cs:[bp+0]
    stosb
    mov     dh, bh
    mov     cl, ah
    rol     dx, cl
    mov     cl, 8
    sub     cl, ah
    mov     ah, 0
    mov     ch, 1
    jmp     short vle_decode_loop_head_08
vle_decode_branch_path_06:
    mov     bh, [si]
    inc     si
    jz      short vle_decode_branch_path_07
vle_decode_loop_head_10:
    mov     ah, 8
    jmp     short vle_decode_loop_head_09
vle_decode_branch_path_07:
    mov     si, ds
    add     si, 1000h
    mov     ds, si
    xor     si, si
    jmp     short vle_decode_loop_head_10
vle_decode_loop_head_11:
    mov     ax, [bp+var_lenlo]
    mov     dx, [bp+var_lenhi]
    add     sp, 200h
    pop     ds
    pop     si
    pop     di
    mov     sp, bp
    pop     bp
    retf
; ASMDOC(font_shapes) Additive branch writes symbol plus previous output modulo 256; its long-code path has unusual
; ASMDOC(font_shapes) SS:BP-relative accesses below.
vle_decode_loop_head_12:
    mov     si, dx
    and     si, 0FFh
    mov     cl, [bp+si+var_lengths]
    cmp     cl, 8
    ja      short vle_decode_branch_path_10
    mov     al, [bp+si+var_symbols]
    add     al, ch
    stosb
    cmp     cl, ah
    jle     short vle_decode_loop_head_13
    xchg    cl, ah
    rol     dx, cl
    xchg    cl, ah
    sub     cl, ah
    jmp     short vle_decode_join_02
    even ; word-align the following entry
vle_decode_loop_head_13:
    rol     dx, cl
    sub     ah, cl
    or      di, di
    jz      short vle_decode_branch_path_08
vle_decode_loop_head_14:
    mov     ch, al
    dec     [bp+var_lenleftlo]
    jnz     short vle_decode_loop_head_12
    dec     [bp+var_lenlefthi]
    jge     short vle_decode_loop_head_12
    jmp     short vle_decode_loop_head_11
vle_decode_branch_path_08:
    mov     di, es
    add     di, 1000h
    mov     es, di
    xor     di, di
    jmp     short vle_decode_loop_head_14
vle_decode_join_02:
    mov     dh, [bx]
    inc     bx
    jz      short vle_decode_branch_path_09
vle_decode_loop_head_15:
    mov     ah, 8
    jmp     short vle_decode_loop_head_13
vle_decode_branch_path_09:
    mov     bx, ds
    add     bx, 1000h
    mov     ds, bx
    xor     bx, bx
    jmp     short vle_decode_loop_head_15
; ASMDOC(font_shapes) Caution: BP becomes 000Eh before [BP-14] stores/compares; these then address SS:0000 or
; later derived SS
; ASMDOC(font_shapes) offsets, not the saved stack-frame local.
vle_decode_branch_path_10:
    push    si
    push    bp
    mov     si, bx
    mov     bp, 0Eh
    mov     bh, dh
    xor     dh, dh
    mov     [bp+var_codoff], 0
vle_decode_loop_head_16:
    cmp     ah, 0
    jz      short vle_decode_branch_path_12
vle_decode_loop_head_17:
    cmp     [bp+var_codoff], 0
    jz      short vle_decode_branch_path_11
    mov     dh, bh
    mov     bx, si
    pop     bp
    pop     si
    jmp     short vle_decode_loop_head_13
vle_decode_branch_path_11:
    shl     bh, 1
    rcl     dx, 1
    sub     ah, 1
    add     bp, 2
    cmp     dx, cs:[bp+423Ch]
    jnb     short vle_decode_loop_head_16
    add     dx, cs:[bp+421Ch]
    mov     bp, dx
    mov     al, cs:[bp+0]
    add     al, ch
    stosb
    mov     dh, bh
    mov     cl, ah
    rol     dx, cl
    mov     cl, 8
    sub     cl, ah
    mov     ah, 0
    mov     [bp+var_codoff], 1
    jmp     short vle_decode_loop_head_16
vle_decode_branch_path_12:
    mov     bh, [si]
    inc     si
    jz      short vle_decode_branch_path_13
vle_decode_loop_head_18:
    mov     ah, 8
    jmp     short vle_decode_loop_head_17
vle_decode_branch_path_13:
    mov     si, ds
    add     si, 1000h
    mov     ds, si
    xor     si, si
    jmp     short vle_decode_loop_head_18
    db 0 ; retained zero fill at this boundary
_file_decomp_vle endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 32FEE. Its internal flow uses noop_entry3_loop_head_01,
; noop_entry3_loop_head_02.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches DX; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): accesses a VGA/MCGA display register port. */
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _nopsub_32FEE (proposed _video_wait_vblank_start): Wait for current
; vertical-retrace bit to
; ASMDOC(font_shapes) clear, then for the next retrace bit to set.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: none in this routine.
; ASMDOC(font_shapes) outputs: AL = final port status byte with bit 3 set; no intended scalar return
; ASMDOC(font_shapes) preserves: AH; BX; CX; SI; DI; BP; DS; ES; SS; CS; SP; DF
; ASMDOC(font_shapes) clobbers: AL; DX (=03DAh); arithmetic flags
; ASMDOC(font_shapes) globals_read: none in this routine.
; ASMDOC(font_shapes) globals_written: none in this routine.
; ASMDOC(font_shapes) services: IN AL,DX at R5HW_VIDEO_STATUS_PORT (03DAh), status bit 3 polling
; ASMDOC(font_shapes) Reachability: No executable token reference or address-taking found in accepted asm/ and
; src/ outside
; ASMDOC(font_shapes) declaration/definition; agrees with existing rename plan. No indirect table reference
; found. Static
; ASMDOC(font_shapes) absence is scoped to accepted sources, not proof about arbitrary external callers.
; ASMDOC(font_shapes) Two busy loops, no timeout; this is active I/O despite the nopsub name.
_nopsub_32FEE proc far

    mov     dx, R5HW_VIDEO_STATUS_PORT
; ASMDOC(font_shapes) Wait for current bit-3 interval to end before waiting for the next rising interval.
noop_entry3_loop_head_01:
    in      al, dx ; Video status bits: /* PLATFORM(video): accesses a VGA/MCGA display register port. */
    test    al, 08h
    jnz     short noop_entry3_loop_head_01
noop_entry3_loop_head_02:
    in      al, dx ; Video status bits: /* PLATFORM(video): accesses a VGA/MCGA display register port. */
    test    al, 08h
    jz      short noop_entry3_loop_head_02
    retf
_nopsub_32FEE endp
; -----------------------------------------------------------------------------
; Purpose: Read the video status register and return its status bits.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches DX; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): accesses a VGA/MCGA display register port. */
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _video_get_status (proposed _video_get_status): Read only the vertical-retrace
; status bit.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: none in this routine.
; ASMDOC(font_shapes) outputs: AX = port 03DAh status & 8 (0 or 8)
; ASMDOC(font_shapes) preserves: BX; CX; SI; DI; BP; DS; ES; SS; CS; SP; DF
; ASMDOC(font_shapes) clobbers: AX; DX (=03DAh); arithmetic flags
; ASMDOC(font_shapes) globals_read: none in this routine.
; ASMDOC(font_shapes) globals_written: none in this routine.
; ASMDOC(font_shapes) services: IN AL,DX at R5HW_VIDEO_STATUS_PORT (03DAh)
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
_video_get_status proc far

    mov     dx, R5HW_VIDEO_STATUS_PORT
    in      al, dx ; Video status bits: /* PLATFORM(video): accesses a VGA/MCGA display register port. */
    and     al, 08h
    xor     ah, ah
    retf
_video_get_status endp

    db 0 ; retained zero fill at this boundary
S012_TEXT ends
end
