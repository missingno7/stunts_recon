; ASMDOC Module review: Existing module name describes patterned spans and window descriptor storage; keep its
; complete mixed
; ASMDOC extent.
; ASMDOC Scope: code-derived contracts below supersede broad legacy summaries without deleting them. No instructions,
; ASMDOC declarations, old comments or line endings changed.
; ASMDOC Public routines: _draw_patterned_lines, _sprite_make_window, _sprite_setup1_from_arg_pointer,
; _sprite_copy_2_to_1. Stack
; ASMDOC offsets are post-prologue BP offsets.
.8086
INCLUDE include\hw.inc
name asm012_150408
SHAPE2D struc
s2d_width dw ?
s2d_height dw ?
s2d_hotspot_x dw ?
s2d_hotspot_y dw ?
s2d_pos_x dw ?
s2d_pos_y dw ?
s2d_plane_color0 db ?
s2d_plane_color1_bg db ?
s2d_plane_color2_flip db ?
s2d_plane_color3_flags db ?
SHAPE2D ends
SPRITE struc
sprite_bitmapptr dd ?
sprite_unk1 dw ?
sprite_unk2 dw ?
sprite_unk3 dw ?
sprite_lineofs dw ?
sprite_left dw ?
sprite_right dw ?
sprite_top dw ?
sprite_height dw ?
sprite_pitch dw ?
sprite_unk4 dw ?
sprite_width2 dw ?
sprite_left2 dw ?
sprite_widthsum dw ?
SPRITE ends
DSEG segment byte public 'STUNTSD'
DSEG ends
_DATA segment word public 'DATA'
aMcgaWindow db 'MCGA WINDOW', 0
aWindowdefOutOfRowTableSpa db 'windowdef - OUT OF ROW TABLE SPACE', 0Dh, 0
_DATA ends
DGROUP group DSEG, _DATA
assume es:nothing, ss:nothing, ds:DSEG
extrn _abortmessage:far
extrn _reservememory:far
extrn _sprite1:byte
extrn _sprite2:byte
extrn _line_pattern_bits:word
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _draw_patterned_lines
public _sprite_make_window
public _next_wnd_def
public _wnd_defs
public _sprite_setup1_from_arg_pointer
public _displaywindow

; ASMDOC Unexported pattern-setting alternate entry: same span arguments as public entry plus BP+10h pattern;
; jumps into common
; ASMDOC core. Normal public entry starts at 63.
    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+10h]
    mov     _line_pattern_bits, ax
    jmp     short pattern_lines_join_01
    even ; word-align the following entry
; ASMDOC Contract _draw_patterned_lines; proposed name _draw_patterned_lines
; ASMDOC Purpose: Draw inclusive spans only where the rotating row-mask bit is 1; bit 0 leaves destination
; unchanged. Swap
; ASMDOC pattern bytes initially when first y is even and after every row.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] near DS offset of signed 16-bit inclusive left endpoints, one word per row; [BP+08]
; near DS offset of
; ASMDOC signed 16-bit inclusive right endpoints, one word per row; [BP+0A] first y word; caller supplies valid
; row-table index;
; ASMDOC [BP+0C] signed row count; zero/negative behavior is entry-specific; [BP+0E] primary color (low byte
; only); DS addresses
; ASMDOC span arrays and pattern globals; no clipping in this entry.
; ASMDOC Outputs: No semantic register result; destination pixels changed; Caller stack slots [BP+06]/[BP+08]
; advance by 2 each
; ASMDOC row; [BP+0C] decremented in place.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF=0.
; ASMDOC Globals read: _sprite1 bitmap segment (+2), CS row-table pointer (+0A) and addressed row offsets;
; _line_pattern_bits (DS
; ASMDOC word; alternating low/high row masks).
; ASMDOC Globals written: _line_pattern_bits; sprite1 backing pixels; caller stack argument slots +06/+08/+0C.
; ASMDOC Services: none.
; ASMDOC Dead: false. Reachability: Address taken in lib/fontmat.ASM:182 and installed in
; _spritefunc; indirect
; ASMDOC span callback, so lack of direct CALL does not imply dead.
; ASMDOC Edge cases: No initial row-count test: zero or negative count still processes one row. Signed
; nonpositive span length
; ASMDOC skips pixels but still advances phase and argument slots.
_draw_patterned_lines label far
; -----------------------------------------------------------------------------
; Purpose: Draw or clear the graphics content identified by draw patterned lines.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12); arg_8
;   (byte ptr 14).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, ES; balanced explicit saves/restores observed: DI,
; SI; path-specific inputs and outputs follow the listed operands.
; Globals read: _line_pattern_bits.
; Globals written: _line_pattern_bits.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
draw_patterned_lines proc far
    arg_left_edges = word ptr 6
    arg_right_edges = word ptr 8
    arg_start_y = word ptr 10
    arg_row_count = word ptr 12
    arg_color = byte ptr 14

    push    bp
    mov     bp, sp
    push    si
    push    di
; ASMDOC Shared span core: swap mask bytes for even first y, align low mask byte to x modulo 8, then consume one
; bit per
; ASMDOC inclusive pixel. Zero count still draws one row.
pattern_lines_join_01:
    cld
    mov     si, [bp+arg_start_y]
    test    si, 01h
    jnz     short pattern_lines_branch_path_01
    mov     ax, _line_pattern_bits
    xchg    ah, al
    mov     _line_pattern_bits, ax
pattern_lines_branch_path_01:
    mov     si, [bp+arg_start_y]
    shl     si, 1
    add     si, cs:_sprite1.sprite_lineofs
    mov     ax, word ptr cs:_sprite1.sprite_bitmapptr+2
    mov     es, ax
    mov     al, [bp+arg_color]
pattern_lines_loop_head_01:
    mov     bx, [bp+arg_right_edges]
    mov     dx, [bx]
    mov     bx, [bp+arg_left_edges]
    mov     bx, [bx]
    mov     ah, byte ptr _line_pattern_bits
    mov     cl, bl
    and     cl, 07h
    rol     ah, cl
    mov     cx, dx
    sub     cx, bx
    inc     cx
    jle     short pattern_lines_loop_head_03
    mov     di, cs:[si]
    add     di, bx
pattern_lines_loop_head_02:
    rol     ah, 1
    jnb     short pattern_lines_branch_path_02
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    loop    pattern_lines_loop_head_02
pattern_lines_loop_head_03:
    add     [bp+arg_right_edges], 2
    add     [bp+arg_left_edges], 2
    add     si, 2
    mov     dx, _line_pattern_bits
    xchg    dh, dl
    mov     _line_pattern_bits, dx
    dec     [bp+arg_row_count]
    jg      short pattern_lines_loop_head_01
    pop     di
    pop     si
    pop     bp
    retf
pattern_lines_branch_path_02:
    inc     di
    loop    pattern_lines_loop_head_02
    jmp     short pattern_lines_loop_head_03
    db 0 ; retained zero fill at this boundary
draw_patterned_lines endp
; ASMDOC Contract _sprite_make_window; proposed name _sprite_make_window
; ASMDOC Purpose: Allocate shape-backed window storage, append a 30-byte sprite descriptor plus height
; row-offset words to the CS
; ASMDOC window arena, and return its far pointer. Does not initialize pixel contents.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] width word; [BP+08] height word; A third color word passed by C callers is unused; DS
; must address
; ASMDOC memory-manager globals; SS is used as DGROUP on fatal arena exhaustion.
; ASMDOC Outputs: DX:AX = CS:descriptor offset on success; Allocated shape header +00 width,+02
; height,+04/+06/+08/+0A zero;
; ASMDOC bytes +0C..+0F and pixels untouched; Rows begin at shape offset 0010h; descriptor bitmap offset is zero.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return; ES.
; ASMDOC Clobbers: AX; BX; CX; DX; arithmetic flags; DF=0 via _mmgr_path_to_name.
; ASMDOC Globals read: _next_wnd_def; CS arena end address; _resptr2/_resendptr1/_resendptr2/_resmaxsize and
; MEMCHUNK records via
; ASMDOC allocator; aMcgaWindow / aWindowdefOutOfRowTableSpa.
; ASMDOC Globals written: _next_wnd_def; _wnd_defs allocated descriptor and rows; allocated shape header;
; ASMDOC _resptr2/_resendptr1/_resmaxsize and MEMCHUNK records via allocator.
; ASMDOC Services: _mmgr_alloc_pages (far; returns segment DX, AX=0; preserves ES/DS/SI/DI/BP); _fatal_error on
; allocation/arena
; ASMDOC exhaustion; nonreturning required.
; ASMDOC Dead: false. Reachability: Many direct C callers, e.g. game/frontend.c:685 and lib/mcgawnd.c:27.
; ASMDOC Edge cases: Uses signed IMUL but only low AX, adds 10h modulo 16 bits, logical shift right 4 then INC
; for paragraph
; ASMDOC count. Arena reservation is 2*(15+height) modulo 16 bits and rejects end >= following code. Height zero
; enters LOOP with
; ASMDOC CX=0 and is not safe. Fatal return would fall into inline data, not a normal epilogue.
_sprite_make_window label far
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprite make wnd.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, SS; balanced explicit saves/restores observed:
; DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: aMcgaWindow, aWindowdefOutOfRowTableSpa.
; -----------------------------------------------------------------------------
sprite_make_wnd proc far
    local_width = word ptr -8
    local_paragraph_count = word ptr -6
    var_4 = word ptr -4
    local_shape_segment = word ptr -2
    arg_width = word ptr 6
    arg_height = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    push    si
    push    di
    mov     ax, [bp+arg_width]
    mov     [bp+local_width], ax
    mov     bx, [bp+arg_height]
    imul    bx
    add     ax, 10h
    mov     cx, 4
    shr     ax, cl
    inc     ax
    mov     bx, ax
    mov     [bp+local_paragraph_count], ax
    push    ax
    mov     ax, offset DGROUP:aMcgaWindow; "MCGA WINDOW"
    push    ax
    call    _reservememory
    add     sp, 4
    mov     [bp+local_shape_segment], dx
    mov     ds, dx
    xor     bx, bx
    mov     ax, [bp+local_width]
    mov     [bx+SHAPE2D.s2d_width], ax
    mov     ax, [bp+arg_height]
    mov     [bx+SHAPE2D.s2d_height], ax
    xor     ax, ax
    mov     [bx+SHAPE2D.s2d_pos_x], ax
    mov     [bx+SHAPE2D.s2d_pos_y], ax
    mov     [bx+SHAPE2D.s2d_hotspot_x], ax
    mov     [bx+SHAPE2D.s2d_hotspot_y], ax
; ASMDOC Reserve descriptor (15 words) plus height row words in CS arena. End equality is rejected. Shape
; allocation has already
; ASMDOC happened; arena fatal handler must not return.
    mov     ax, 0Fh         ; sizeof SPRITE (in words)
    add     ax, [bp+arg_height]  ; height ??
    shl     ax, 1
    mov     bx, cs:next_wnd_def
    mov     [bp+var_4], bx
    add     ax, bx
    cmp     ax, offset sprite_set_1_from_argptr
; sprite_set_1 happens to be the end of the buffer.. see below this func
    jnb     short make_window_error_path_01
    mov     cs:next_wnd_def, ax
    mov     word ptr cs:[bx+SPRITE.sprite_bitmapptr], 0
    mov     ax, [bp+local_shape_segment]
    mov      word ptr cs:[bx+(SPRITE.sprite_bitmapptr+2)], ax
    lea     ax, [bx+(size SPRITE)]
    mov     cs:[bx+SPRITE.sprite_lineofs], ax
    mov     cs:[bx+SPRITE.sprite_left], 0
    mov     cs:[bx+SPRITE.sprite_left2], 0
    mov     ax, [bp+local_width]
    mov     cs:[bx+SPRITE.sprite_right], ax
    mov     cs:[bx+SPRITE.sprite_pitch], ax
    mov     cs:[bx+SPRITE.sprite_top], 0
    mov     ax, [bp+arg_height]
    mov     cs:[bx+SPRITE.sprite_height], ax
    mov     ax, [bp+arg_width]
    mov     cs:[bx+SPRITE.sprite_width2], ax
    mov     cs:[bx+SPRITE.sprite_widthsum], ax
    mov     cx, [bp+arg_height]
    mov     ax, size SHAPE2D
make_window_loop_head_01:
    mov     cs:[bx+(size SPRITE)], ax
    add     bx, 2
    add     ax, [bp+local_width]
    loop    make_window_loop_head_01
    mov     dx, cs
    mov     ax, [bp+var_4]
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
make_window_error_path_01:
    mov     ax, ss
    mov     ds, ax
    mov     ax, offset DGROUP:aWindowdefOutOfRowTableSpa; "windowdef - OUT OF ROW TABLE SPACE\r"
    push    ax
    call    far ptr _abortmessage
    even ; word-align the following entry
; ASMDOC PUBLIC DATA, not entry point: next_wnd_def stores next free CS offset. PUBLIC wnd_defs below is a 3600-byte
; ASMDOC descriptor/row arena ending at following public code label.
_next_wnd_def label word
next_wnd_def     dw offset wnd_defs
_wnd_defs label byte
; Initialized storage: 3600 byte elements, each 0; verified all entries.
wnd_defs label byte
    REPT 3600
    db 00h
    ENDM
sprite_make_wnd endp
; ASMDOC Contract _sprite_setup1_from_arg_pointer; proposed name _sprite_setup1_from_arg_pointer
; ASMDOC Purpose: Copy exactly 15 words of a caller-supplied sprite descriptor into CS:_sprite1; no pixels are copied.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] descriptor offset; [BP+08] descriptor segment; source need not be DGROUP; Source
; row-table offset is
; ASMDOC copied verbatim and subsequent renderers dereference it in CS.
; ASMDOC Outputs: CS:_sprite1 receives 30 bytes; ES=CS; AX=CS; CX=0.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return; BX; DX.
; ASMDOC Clobbers: AX; CX; ES; DF=0; other arithmetic flags unchanged.
; ASMDOC Globals read: caller far descriptor (30 bytes).
; ASMDOC Globals written: _sprite1 descriptor (30 bytes).
; ASMDOC Services: none.
; ASMDOC Dead: false. Reachability: Direct callers game/frontend.c:2540, game/race.c:1512 and internal call at
; ASMDOC patlines.ASM:3895.
; ASMDOC End of CS window arena and start of callable descriptor-copy entry; reserving through this address is
; rejected.
_sprite_setup1_from_arg_pointer label far
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprite set 1 from argptr.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, CX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS,
; SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _sprite1.
; -----------------------------------------------------------------------------
sprite_set_1_from_argptr proc far
    arg_descriptor_offset = word ptr 6
    arg_descriptor_segment = word ptr 8

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    cld
    mov     ds, [bp+arg_descriptor_segment]
    mov     si, [bp+arg_descriptor_offset]
    mov     cx, 0Fh
    mov     ax, cs
    mov     es, ax
    mov     di, offset _sprite1
    rep movsw
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
sprite_set_1_from_argptr endp
; ASMDOC Contract _sprite_copy_2_to_1; proposed name _sprite_copy_2_to_1
; ASMDOC Purpose: Select sprite2 by calling the descriptor copier; copy 30 descriptor bytes from sprite2 into sprite1.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV
; BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: No arguments.
; ASMDOC Outputs: CS:_sprite1 receives CS:_sprite2 descriptor; AX=CS; CX=0; ES=CS.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return; BX; DX.
; ASMDOC Clobbers: AX; CX; ES; arithmetic flags from ADD SP; DF=0.
; ASMDOC Globals read: _sprite2 descriptor.
; ASMDOC Globals written: _sprite1 descriptor.
; ASMDOC Services: sprite_set_1_from_argptr / _sprite_setup1_from_arg_pointer.
; ASMDOC Dead: false. Reachability: Direct callers game/race.c:1515 and lib/gfxres.ASM:237.
_displaywindow label far
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprite copy 2 to 1.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs
; and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _sprite2.
; -----------------------------------------------------------------------------
displaywindow proc far

    mov     ax, seg S012_TEXT
    push    ax
    mov     ax, offset _sprite2
    push    ax
    call    sprite_set_1_from_argptr
    add     sp, 4
    retf
    db 0 ; retained zero fill at this boundary
displaywindow endp
S012_TEXT ends
end
