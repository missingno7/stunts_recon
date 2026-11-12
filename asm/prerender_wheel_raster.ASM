; ASMDOC-GEOMETRY Module: prerender_wheel_raster. Existing content name remains useful; includes shared
; polygon raster workers and alternate record-list entries. No merge or split proposed.
; ASMDOC-GEOMETRY Original source: asm/prerender_wheel_raster.ASM; contracts below use ORIGINAL line
; references.
; ASMDOC-GEOMETRY Code exports: _prerender_shared_setup, _preRender_default, _preRender_default_alt,
; _skybox_op_helper, _preRender_wheel_helper4, _preRender_helper, _preRender_helper2, _preRender_helper3,
; _nopsub_31F39, _nopsub_31F55
; ASMDOC-GEOMETRY All arguments/coordinates are 16-bit unless explicitly stated. BP offsets are after PUSH
; BP/MOV BP,SP.
; ASMDOC-GEOMETRY The detailed comments describe control flow including shared tails and transitive services;
; old generic comments are retained.
; ASMDOC-GEOMETRY Data export: _line_pattern_bits: u16 initialized 0; pattern callback rotates its byte phase
; ASMDOC-GEOMETRY Data export: _prerender_auxiliary_arg: u16 initialized 0, used by auxiliary fill callback
; ASMDOC-GEOMETRY Data export: _spritefunc: offset=0, segment=S012_TEXT; FAR span callback
; ASMDOC-GEOMETRY Data export: _imagefunc: offset=0, segment=S012_TEXT; FAR degenerate-line callback
; ASMDOC-GEOMETRY 
INCLUDE include\platform_hw.inc
.model medium
name asm012_137138
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
SPRITE_LEFT2_OFFSET equ 26
SPRITE_WIDTHSUM_OFFSET equ 28
SPRITE_TOP_OFFSET equ 16
SPRITE_HEIGHT_OFFSET equ 18
_DATA segment word public 'DATA'
public _line_pattern_bits, _prerender_auxiliary_arg, _spritefunc, _imagefunc
_line_pattern_bits dw 0
_prerender_auxiliary_arg dw 0
_spritefunc dw 0
    dw seg S012_TEXT
_imagefunc dw 0
    dw seg S012_TEXT
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
extrn _sprite1:word
extrn _draw_filled_lines:far
extrn _preRender_line:far
extrn _clipline:far
extrn _slopeline:far
spritefunc equ _spritefunc
imagefunc equ _imagefunc
draw_filled_lines equ _draw_filled_lines
preRender_line equ _preRender_line
clipline equ _clipline
slopeline equ _slopeline
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _polycmn
public _drawpolypt
public _drawpolyp
public _skybox_op_helper
public _preRender_wheel_helper4
public _preRender_helper
public _preRender_helper2
public _preRender_helper3
public _sprite_fill_line_records
public _sprite_fill_line_records_alt
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _preRender_default (original line 61)
; ASMDOC-GEOMETRY Purpose: Rasterize a cyclic 2D polygon with solid spans; merge mode 1; vertices are a near
; array.
; ASMDOC-GEOMETRY Proposed public: _preRender_default
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: color/style passed unchanged to selected callback (solid fill uses AL)
; ASMDOC-GEOMETRY Input: BP+8 word: positive vertex count; no zero/negative-count guard
; ASMDOC-GEOMETRY Input: DS=SS=DGROUP for near stack scratch pointers; normal sprite1 geometry requires
; scanline indices inside the 480-word scratch arrays
; ASMDOC-GEOMETRY Input: BP+10 word: near DS pointer to count POINT2D {x:i16,y:i16} records; MOV SI,[BP+10]
; ASMDOC-GEOMETRY Output: Writes indexed pixels of current sprite1 through callbacks; no scalar return
; contract
; ASMDOC-GEOMETRY Output: Scratch record BP-2000..BP-1973; left array BP-1944+2*y; right array BP-984+2*y (480
; words each); caller frame and arguments remain except callback-local stack copies
; ASMDOC-GEOMETRY Output: Sets DF=0; ES becomes SS during setup and may subsequently become selected sprite
; bitmap segment in callbacks
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: ES
; ASMDOC-GEOMETRY Clobbers: FLAGS (DF cleared)
; ASMDOC-GEOMETRY Global read: _sprite1 clip words +26 left,+28 right-exclusive,+16 top,+18 bottom-exclusive
; ASMDOC-GEOMETRY Global read: _spritefunc and _imagefunc far callback vectors
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global write: _spritefunc.offset = OFFSET _draw_filled_lines; segment retains initialized
; S012_TEXT
; ASMDOC-GEOMETRY Global write: _imagefunc.offset = OFFSET _preRender_line; segment retains initialized
; S012_TEXT
; ASMDOC-GEOMETRY Global write: current sprite1 pixel backing (transitively)
; ASMDOC-GEOMETRY Service/call: _draw_line_related FAR (clipped 28-byte line-record builder)
; ASMDOC-GEOMETRY Service/call: _draw_line_related_alt FAR (unclipped record builder)
; ASMDOC-GEOMETRY Service/call: _preRender_helper / _preRender_helper2 / _preRender_helper3 NEAR register
; workers
; ASMDOC-GEOMETRY Service/call: FAR [_imagefunc]: normally _preRender_line on point/axis-degenerate input
; ASMDOC-GEOMETRY Service/call: FAR [_spritefunc]: normally
; _draw_filled_lines(left*,right*,start_y,count,color)
; ASMDOC-GEOMETRY Detail: Allocates 0x7D0 bytes plus saved SI/DI; mode stored at byte BP-10. Enters shared
; setup without a second call frame.
; ASMDOC-GEOMETRY Detail: Count 1 calls the line callback with identical endpoints before bounding-box
; rejection; horizontal/vertical polygons call the line callback with bounding endpoints.
; ASMDOC-GEOMETRY Detail: Strict right rejection is min_x >= (right-1), not min_x >= right; nondegenerate
; clipped vertical extent must contain at least two rows (SUB end,start; JLE return; INC count).
; ASMDOC-GEOMETRY Detail: Near workers use code-resident mode tables; not I/O instructions. No DOS/BIOS/port
; call in this module.
; ASMDOC-GEOMETRY Dead: false. Reachability: src/obj_seg006.c:667 direct polygon call
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_drawpolypt label far
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender default.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: no named stack arguments referenced.
; Register ABI: touches SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
drawpolypt proc far
    local_merge_mode = byte ptr -10
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     [bp+local_merge_mode], 1
    jmp     short pre_default_alt_join_01
drawpolypt endp
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _preRender_default_alt (original line 81)
; ASMDOC-GEOMETRY Purpose: Rasterize a cyclic 2D polygon with solid spans; merge mode 0; vertices are a near
; array.
; ASMDOC-GEOMETRY Proposed public: _preRender_default_alt
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: color/style passed unchanged to selected callback (solid fill uses AL)
; ASMDOC-GEOMETRY Input: BP+8 word: positive vertex count; no zero/negative-count guard
; ASMDOC-GEOMETRY Input: DS=SS=DGROUP for near stack scratch pointers; normal sprite1 geometry requires
; scanline indices inside the 480-word scratch arrays
; ASMDOC-GEOMETRY Input: BP+10 word: near DS pointer to count POINT2D {x:i16,y:i16} records; MOV SI,[BP+10]
; ASMDOC-GEOMETRY Output: Writes indexed pixels of current sprite1 through callbacks; no scalar return
; contract
; ASMDOC-GEOMETRY Output: Scratch record BP-2000..BP-1973; left array BP-1944+2*y; right array BP-984+2*y (480
; words each); caller frame and arguments remain except callback-local stack copies
; ASMDOC-GEOMETRY Output: Sets DF=0; ES becomes SS during setup and may subsequently become selected sprite
; bitmap segment in callbacks
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: ES
; ASMDOC-GEOMETRY Clobbers: FLAGS (DF cleared)
; ASMDOC-GEOMETRY Global read: _sprite1 clip words +26 left,+28 right-exclusive,+16 top,+18 bottom-exclusive
; ASMDOC-GEOMETRY Global read: _spritefunc and _imagefunc far callback vectors
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global write: _spritefunc.offset = OFFSET _draw_filled_lines; segment retains initialized
; S012_TEXT
; ASMDOC-GEOMETRY Global write: _imagefunc.offset = OFFSET _preRender_line; segment retains initialized
; S012_TEXT
; ASMDOC-GEOMETRY Global write: current sprite1 pixel backing (transitively)
; ASMDOC-GEOMETRY Service/call: _draw_line_related FAR (clipped 28-byte line-record builder)
; ASMDOC-GEOMETRY Service/call: _draw_line_related_alt FAR (unclipped record builder)
; ASMDOC-GEOMETRY Service/call: _preRender_helper / _preRender_helper2 / _preRender_helper3 NEAR register
; workers
; ASMDOC-GEOMETRY Service/call: FAR [_imagefunc]: normally _preRender_line on point/axis-degenerate input
; ASMDOC-GEOMETRY Service/call: FAR [_spritefunc]: normally
; _draw_filled_lines(left*,right*,start_y,count,color)
; ASMDOC-GEOMETRY Detail: Allocates 0x7D0 bytes plus saved SI/DI; mode stored at byte BP-10. Enters shared
; setup without a second call frame.
; ASMDOC-GEOMETRY Detail: Count 1 calls the line callback with identical endpoints before bounding-box
; rejection; horizontal/vertical polygons call the line callback with bounding endpoints.
; ASMDOC-GEOMETRY Detail: Strict right rejection is min_x >= (right-1), not min_x >= right; nondegenerate
; clipped vertical extent must contain at least two rows (SUB end,start; JLE return; INC count).
; ASMDOC-GEOMETRY Detail: Near workers use code-resident mode tables; not I/O instructions. No DOS/BIOS/port
; call in this module.
; ASMDOC-GEOMETRY Dead: false. Reachability: src/preRender_wheel.c:82,104,106 and
; src/preRender_sphere_helper.c:16
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_drawpolyp label far
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender default alt.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: arg_6 (word ptr 10).
; Register ABI: touches AX, SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
drawpolyp proc far
    local_merge_mode = byte ptr -10
    arg_vertices = word ptr 10
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     [bp+local_merge_mode], 0
pre_default_alt_join_01:
    mov     ax, offset draw_filled_lines
    mov     spritefunc, ax
    mov     ax, offset preRender_line
    mov word ptr imagefunc, ax
    mov     si, [bp+arg_vertices]
    jmp     short _polycmn
drawpolyp endp
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _skybox_op_helper (original line 108)
; ASMDOC-GEOMETRY Purpose: Rasterize a cyclic 2D polygon with solid spans; merge mode 1; vertices are inline
; stack words.
; ASMDOC-GEOMETRY Proposed public: _skybox_op_helper
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: color/style passed unchanged to selected callback (solid fill uses AL)
; ASMDOC-GEOMETRY Input: BP+8 word: positive vertex count; no zero/negative-count guard
; ASMDOC-GEOMETRY Input: DS=SS=DGROUP for near stack scratch pointers; normal sprite1 geometry requires
; scanline indices inside the 480-word scratch arrays
; ASMDOC-GEOMETRY Input: BP+10 onward: count inline POINT2D {x:i16,y:i16} records; LEA SI,[BP+10]
; ASMDOC-GEOMETRY Output: Writes indexed pixels of current sprite1 through callbacks; no scalar return
; contract
; ASMDOC-GEOMETRY Output: Scratch record BP-2000..BP-1973; left array BP-1944+2*y; right array BP-984+2*y (480
; words each); caller frame and arguments remain except callback-local stack copies
; ASMDOC-GEOMETRY Output: Sets DF=0; ES becomes SS during setup and may subsequently become selected sprite
; bitmap segment in callbacks
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: ES
; ASMDOC-GEOMETRY Clobbers: FLAGS (DF cleared)
; ASMDOC-GEOMETRY Global read: _sprite1 clip words +26 left,+28 right-exclusive,+16 top,+18 bottom-exclusive
; ASMDOC-GEOMETRY Global read: _spritefunc and _imagefunc far callback vectors
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global write: _spritefunc.offset = OFFSET _draw_filled_lines; segment retains initialized
; S012_TEXT
; ASMDOC-GEOMETRY Global write: _imagefunc.offset = OFFSET _preRender_line; segment retains initialized
; S012_TEXT
; ASMDOC-GEOMETRY Global write: current sprite1 pixel backing (transitively)
; ASMDOC-GEOMETRY Service/call: _draw_line_related FAR (clipped 28-byte line-record builder)
; ASMDOC-GEOMETRY Service/call: _draw_line_related_alt FAR (unclipped record builder)
; ASMDOC-GEOMETRY Service/call: _preRender_helper / _preRender_helper2 / _preRender_helper3 NEAR register
; workers
; ASMDOC-GEOMETRY Service/call: FAR [_imagefunc]: normally _preRender_line on point/axis-degenerate input
; ASMDOC-GEOMETRY Service/call: FAR [_spritefunc]: normally
; _draw_filled_lines(left*,right*,start_y,count,color)
; ASMDOC-GEOMETRY Detail: Allocates 0x7D0 bytes plus saved SI/DI; mode stored at byte BP-10. Enters shared
; setup without a second call frame.
; ASMDOC-GEOMETRY Detail: Count 1 calls the line callback with identical endpoints before bounding-box
; rejection; horizontal/vertical polygons call the line callback with bounding endpoints.
; ASMDOC-GEOMETRY Detail: Strict right rejection is min_x >= (right-1), not min_x >= right; nondegenerate
; clipped vertical extent must contain at least two rows (SUB end,start; JLE return; INC count).
; ASMDOC-GEOMETRY Detail: Near workers use code-resident mode tables; not I/O instructions. No DOS/BIOS/port
; call in this module.
; ASMDOC-GEOMETRY Dead: false. Reachability: src/obj_seg003.c:2238-2239 passes four POINT2D values inline
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_skybox_op_helper label far
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by skybox op helper.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: no named stack arguments referenced.
; Register ABI: touches SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
skybox_op_helper proc far
    local_merge_mode = byte ptr -10
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     [bp+local_merge_mode], 1
    jmp     short wheel_raster_join_01
skybox_op_helper endp
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _preRender_wheel_helper4 (original line 128)
; ASMDOC-GEOMETRY Purpose: Rasterize a cyclic 2D polygon with solid spans; merge mode 0; vertices are inline
; stack words.
; ASMDOC-GEOMETRY Proposed public: _preRender_wheel_helper4
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: color/style passed unchanged to selected callback (solid fill uses AL)
; ASMDOC-GEOMETRY Input: BP+8 word: positive vertex count; no zero/negative-count guard
; ASMDOC-GEOMETRY Input: DS=SS=DGROUP for near stack scratch pointers; normal sprite1 geometry requires
; scanline indices inside the 480-word scratch arrays
; ASMDOC-GEOMETRY Input: BP+10 onward: count inline POINT2D {x:i16,y:i16} records; LEA SI,[BP+10]
; ASMDOC-GEOMETRY Output: Writes indexed pixels of current sprite1 through callbacks; no scalar return
; contract
; ASMDOC-GEOMETRY Output: Scratch record BP-2000..BP-1973; left array BP-1944+2*y; right array BP-984+2*y (480
; words each); caller frame and arguments remain except callback-local stack copies
; ASMDOC-GEOMETRY Output: Sets DF=0; ES becomes SS during setup and may subsequently become selected sprite
; bitmap segment in callbacks
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: ES
; ASMDOC-GEOMETRY Clobbers: FLAGS (DF cleared)
; ASMDOC-GEOMETRY Global read: _sprite1 clip words +26 left,+28 right-exclusive,+16 top,+18 bottom-exclusive
; ASMDOC-GEOMETRY Global read: _spritefunc and _imagefunc far callback vectors
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global write: _spritefunc.offset = OFFSET _draw_filled_lines; segment retains initialized
; S012_TEXT
; ASMDOC-GEOMETRY Global write: _imagefunc.offset = OFFSET _preRender_line; segment retains initialized
; S012_TEXT
; ASMDOC-GEOMETRY Global write: current sprite1 pixel backing (transitively)
; ASMDOC-GEOMETRY Service/call: _draw_line_related FAR (clipped 28-byte line-record builder)
; ASMDOC-GEOMETRY Service/call: _draw_line_related_alt FAR (unclipped record builder)
; ASMDOC-GEOMETRY Service/call: _preRender_helper / _preRender_helper2 / _preRender_helper3 NEAR register
; workers
; ASMDOC-GEOMETRY Service/call: FAR [_imagefunc]: normally _preRender_line on point/axis-degenerate input
; ASMDOC-GEOMETRY Service/call: FAR [_spritefunc]: normally
; _draw_filled_lines(left*,right*,start_y,count,color)
; ASMDOC-GEOMETRY Detail: Allocates 0x7D0 bytes plus saved SI/DI; mode stored at byte BP-10. Enters shared
; setup without a second call frame.
; ASMDOC-GEOMETRY Detail: Count 1 calls the line callback with identical endpoints before bounding-box
; rejection; horizontal/vertical polygons call the line callback with bounding endpoints.
; ASMDOC-GEOMETRY Detail: Strict right rejection is min_x >= (right-1), not min_x >= right; nondegenerate
; clipped vertical extent must contain at least two rows (SUB end,start; JLE return; INC count).
; ASMDOC-GEOMETRY Detail: Near workers use code-resident mode tables; not I/O instructions. No DOS/BIOS/port
; call in this module.
; ASMDOC-GEOMETRY Dead: false. Reachability: src/preRender_wheel.c:37,45 passes four x/y pairs inline
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_preRender_wheel_helper4 label far
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender wheel helper4.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_color (word ptr 6);
; arg_vertlinecount (word ptr 8); arg_vertlines (byte ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, ES, SS; balanced explicit saves/restores observed:
; DI, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
preRender_wheel_helper4 proc far
    local_line_descriptor = word ptr -2000
    local_left_edges = word ptr -1944
    local_right_edges = word ptr -984
    local_edge_cursor = word ptr -24
    var_16 = word ptr -22
    local_bottom_entry = word ptr -20
    local_bottom_y = word ptr -18
    local_top_entry = word ptr -16
    local_top_y = word ptr -14
    local_clip_required = byte ptr -12
    local_merge_mode = byte ptr -10
    local_last_list_entry = word ptr -8
    var_vertlineptr = word ptr -6
    local_clip_right_inclusive = word ptr -4
    local_clip_left = word ptr -2
    arg_color = word ptr 6
    arg_vertlinecount = word ptr 8
    arg_vertlines = byte ptr 10
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     [bp+local_merge_mode], 0
; ASMDOC-GEOMETRY Inline wrappers obtain vertices with LEA, whereas default wrappers MOV a near pointer; both
; preserve one FAR wrapper frame.
wheel_raster_join_01:
    mov     ax, offset draw_filled_lines
    mov     spritefunc, ax
    mov     ax, offset preRender_line
    mov word ptr imagefunc, ax
    lea     si, [bp+arg_vertlines]
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _prerender_shared_setup (original line 170)
; ASMDOC-GEOMETRY Purpose: Interior polygon-raster continuation shared by solid, patterned, and auxiliary-
; color entry wrappers.
; ASMDOC-GEOMETRY Proposed public: _prerender_shared_setup
; ASMDOC-GEOMETRY Convention: NEAR jump target inside preRender_wheel_helper4, not a callable C function; no
; prologue, no new return address; shared exits POP DI/POP SI/MOV SP,BP/POP BP/RETF unwind the wrapper
; ASMDOC-GEOMETRY Input: BP+6 word: color/style passed unchanged to selected callback (solid fill uses AL)
; ASMDOC-GEOMETRY Input: BP+8 word: positive vertex count; no zero/negative-count guard
; ASMDOC-GEOMETRY Input: DS=SS=DGROUP for near stack scratch pointers; normal sprite1 geometry requires
; scanline indices inside the 480-word scratch arrays
; ASMDOC-GEOMETRY Input: SI: near DS pointer to count POINT2D records
; ASMDOC-GEOMETRY Input: BP points to original FAR wrapper frame; SP=BP-0x7D4, with saved SI at BP-0x7D2 and
; DI at BP-0x7D4
; ASMDOC-GEOMETRY Input: byte [SS:BP-10]: merge mode (0 or 1)
; ASMDOC-GEOMETRY Input: _spritefunc/_imagefunc: valid initialized FAR callback vectors; segment words are not
; installed here
; ASMDOC-GEOMETRY Output: Writes indexed pixels of current sprite1 through callbacks; no scalar return
; contract
; ASMDOC-GEOMETRY Output: Scratch record BP-2000..BP-1973; left array BP-1944+2*y; right array BP-984+2*y (480
; words each); caller frame and arguments remain except callback-local stack copies
; ASMDOC-GEOMETRY Output: Sets DF=0; ES becomes SS during setup and may subsequently become selected sprite
; bitmap segment in callbacks
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: ES
; ASMDOC-GEOMETRY Clobbers: FLAGS (DF cleared)
; ASMDOC-GEOMETRY Global read: _sprite1 clip words +26 left,+28 right-exclusive,+16 top,+18 bottom-exclusive
; ASMDOC-GEOMETRY Global read: _spritefunc and _imagefunc far callback vectors
; ASMDOC-GEOMETRY Global read: _line_pattern_bits / _prerender_auxiliary_arg only through externally selected
; callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global write: current sprite1 pixel backing via chosen callback
; ASMDOC-GEOMETRY Global write: _line_pattern_bits may be rotated by _draw_patterned_lines callback
; ASMDOC-GEOMETRY Service/call: _draw_line_related FAR (clipped 28-byte line-record builder)
; ASMDOC-GEOMETRY Service/call: _draw_line_related_alt FAR (unclipped record builder)
; ASMDOC-GEOMETRY Service/call: _preRender_helper / _preRender_helper2 / _preRender_helper3 NEAR register
; workers
; ASMDOC-GEOMETRY Service/call: FAR [_imagefunc]: normally _preRender_line on point/axis-degenerate input
; ASMDOC-GEOMETRY Service/call: FAR [_spritefunc]: normally
; _draw_filled_lines(left*,right*,start_y,count,color)
; ASMDOC-GEOMETRY Service/call: caller-selected _draw_patterned_lines or _draw_unknown_lines through
; _spritefunc
; ASMDOC-GEOMETRY Detail: Cross-module jump is live, despite asm-migration marking
; NO_INDEXED_C_PATH/INDIRECT_OR_LINK_USE_UNRESOLVED.
; ASMDOC-GEOMETRY Detail: Neither establishes callback vectors nor allocates its frame; inherited wrapper
; frame is mandatory.
; ASMDOC-GEOMETRY Dead: false. Reachability: Local jumps at asm/prerender_wheel_raster.ASM:106,170; external
; near jumps at asm/font_matrix_shape_decode.ASM:186 and asm/graphics_resource_runtime.ASM:1545
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_polycmn:
    mov     [bp+var_vertlineptr], si
    mov     cx, [bp+arg_vertlinecount]
    mov     ax, cx
    dec     ax
    shl     ax, 1
    shl     ax, 1
    add     ax, si
    mov     [bp+local_last_list_entry], ax
    cld
    mov     ax, ss
    mov     es, ax
    lea     ax, [bp+local_left_edges]
    mov     [bp+local_edge_cursor], ax
    mov     ax, cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    mov     [bp+local_clip_left], ax
    mov     ax, cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    dec     ax
    mov     [bp+local_clip_right_inclusive], ax
    mov     ax, [si+2]
    mov     [bp+local_top_y], ax
    mov     [bp+local_bottom_y], ax
    mov     bx, [si]
    mov     dx, bx
    mov     [bp+local_top_entry], si
    mov     [bp+local_bottom_entry], si
    add     si, 4
    dec     cx
    jnz     short wheel_raster_loop_head_01
    push    [bp+arg_color]
    mov     si, [bp+var_vertlineptr]
    push    word ptr [si+2]
    push    word ptr [si]
    push    word ptr [si+2]
    push    word ptr [si]
    call    dword ptr imagefunc
    add     sp, 0Ah
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
; ASMDOC-GEOMETRY Find signed bounding box and first/last y extrema; equal minima replace the chosen top
; vertex. No source count validation.
wheel_raster_loop_head_01:
    mov     ax, [si+2]
    cmp     ax, [bp+local_top_y]
    jg      short wheel_raster_branch_path_01
    mov     [bp+local_top_y], ax
    mov     [bp+local_top_entry], si
wheel_raster_branch_path_01:
    cmp     ax, [bp+local_bottom_y]
    jle     short wheel_raster_branch_path_02
    mov     [bp+local_bottom_y], ax
    mov     [bp+local_bottom_entry], si
wheel_raster_branch_path_02:
    mov     ax, [si]
    cmp     ax, bx
    jge     short wheel_raster_branch_path_03
    mov     bx, ax
wheel_raster_branch_path_03:
    cmp     ax, dx
    jle     short wheel_raster_branch_path_04
    mov     dx, ax
wheel_raster_branch_path_04:
    add     si, 4
    loop    wheel_raster_loop_head_01
    cmp     dx, [bp+local_clip_left]
    jl      short wheel_raster_branch_path_08
    cmp     bx, [bp+local_clip_right_inclusive]
    jge     short wheel_raster_branch_path_08
    mov     ax, [bp+local_bottom_y]
    cmp     ax, cs:[_sprite1+SPRITE_TOP_OFFSET]
    jl      short wheel_raster_branch_path_08
    mov     cx, [bp+local_top_y]
    cmp     cx, cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jge     short wheel_raster_branch_path_08
    mov     [bp+local_clip_required], 0
    cmp     dx, [bp+local_clip_right_inclusive]
    jg      short wheel_raster_branch_path_05
    cmp     bx, [bp+local_clip_left]
    jl      short wheel_raster_branch_path_05
    cmp     ax, cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jge     short wheel_raster_branch_path_05
    cmp     cx, cs:[_sprite1+SPRITE_TOP_OFFSET]
    jge     short wheel_raster_branch_path_06
wheel_raster_branch_path_05:
    mov     [bp+local_clip_required], 1
wheel_raster_branch_path_06:
    cmp     ax, cx
    jz      short wheel_raster_branch_path_07
    cmp     dx, bx
    jnz     short wheel_raster_branch_path_09
wheel_raster_branch_path_07:
    push    [bp+arg_color]
    mov     si, [bp+var_vertlineptr]
    push    ax
    push    dx
    push    cx
    push    bx
    call    dword ptr imagefunc
    add     sp, 0Ah
wheel_raster_branch_path_08:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
; ASMDOC-GEOMETRY Walk opposite sides of cyclic vertex ring; only rising-y edges emit records. First chain
; seeds arrays, second merges them.
wheel_raster_branch_path_09:
    lea     si, [bp+local_line_descriptor]
    mov     di, [bp+local_top_entry]
wheel_raster_loop_head_02:
    mov     ax, [di+2]
    mov     cx, [di]
    add     di, 4
    cmp     di, [bp+local_last_list_entry]
    jbe     short wheel_raster_branch_path_10
    mov     di, [bp+var_vertlineptr]
wheel_raster_branch_path_10:
    mov     dx, [di+2]
    cmp     dx, ax
    jle     short wheel_raster_branch_path_12
    push    si
    push    dx
    push    word ptr [di]
    push    ax
    push    cx
    cmp     [bp+local_clip_required], 0
    jz      short wheel_raster_branch_path_11
    call    clipline
    mov     [bp+var_16], di
    call near ptr preRender_helper
    jmp     short wheel_raster_join_02
    even ; word-align the following entry
wheel_raster_branch_path_11:
    call    slopeline
    mov     [bp+var_16], di
    call near ptr preRender_helper2
wheel_raster_join_02:
    mov     di, [bp+var_16]
    add     sp, 0Ah
wheel_raster_branch_path_12:
    cmp     di, [bp+local_bottom_entry]
    jnz     short wheel_raster_loop_head_02
    mov     di, [bp+local_top_entry]
wheel_raster_loop_head_03:
    mov     ax, [di+2]
    mov     cx, [di]
    sub     di, 4
    cmp     di, [bp+var_vertlineptr]
    jnb     short wheel_raster_branch_path_13
    mov     di, [bp+local_last_list_entry]
wheel_raster_branch_path_13:
    mov     dx, [di+2]
    cmp     dx, ax
    jle     short wheel_raster_branch_path_15
    push    si
    push    dx
    push    word ptr [di]
    push    ax
    push    cx
    cmp     [bp+local_clip_required], 0
    jz      short wheel_raster_branch_path_14
    call    clipline
    jmp     short wheel_raster_join_03
    even ; word-align the following entry
wheel_raster_branch_path_14:
    call    slopeline
wheel_raster_join_03:
    mov     [bp+var_16], di
    call near ptr preRender_helper3
    mov     di, [bp+var_16]
    add     sp, 0Ah
wheel_raster_branch_path_15:
    cmp     di, [bp+local_bottom_entry]
    jnz     short wheel_raster_loop_head_03
    mov     ax, [bp+local_bottom_y]
    cmp     ax, cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jl      short wheel_raster_branch_path_16
    mov     ax, cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    dec     ax
wheel_raster_branch_path_16:
    mov     bx, [bp+local_top_y]
    cmp     bx, cs:[_sprite1+SPRITE_TOP_OFFSET]
    jge     short wheel_raster_branch_path_17
    mov     bx, cs:[_sprite1+SPRITE_TOP_OFFSET]
wheel_raster_branch_path_17:
    sub     ax, bx
    jle     short wheel_raster_branch_path_18
    inc     ax
    push    [bp+arg_color]
    push    ax
    push    bx
    shl     bx, 1
    lea     ax, [bp+local_right_edges]
    add     ax, bx
    push    ax
    lea     ax, [bp+local_left_edges]
    add     ax, bx
    push    ax
    call    dword ptr spritefunc
    add     sp, 0Ah
wheel_raster_branch_path_18:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
preRender_wheel_helper4 endp
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _preRender_helper (original line 377)
; ASMDOC-GEOMETRY Purpose: Prefill clipped rows of both boundary arrays with empty spans, then fall through to
; _preRender_helper2.
; ASMDOC-GEOMETRY Proposed public: _preRender_helper
; ASMDOC-GEOMETRY Convention: NEAR register worker; BP is inherited outer frame, not worker frame; RETN
; returns to internal caller
; ASMDOC-GEOMETRY Input: DS:SI: 28-byte prepared line record: +0 x fraction u16,+2 start x i16,+4 y fraction
; u16,+6 start y i16,+8 end x i16,+10 end y i16,+12 slope u16,+14 sample count i16,+16 color,+18 mode byte
; (0..9),+19 clip flags,+20/+22 top/bottom rows,+24/+26 left/right rows
; ASMDOC-GEOMETRY Input: SS:BP-24 word: base near offset of left scanline array; right array is base+0x3C0;
; absolute y indexes into these arrays
; ASMDOC-GEOMETRY Input: ES=DS=SS, DF=0; BP remains outer polygon frame; NEAR CALL supplies only a return IP,
; no stack arguments
; ASMDOC-GEOMETRY Output: Updates ES scanline arrays; no scalar return; line record and SI unchanged
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: ES
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: DI
; ASMDOC-GEOMETRY Clobbers: FLAGS
; ASMDOC-GEOMETRY Global read: _sprite1 +26/+28 clip edges
; ASMDOC-GEOMETRY Service/call: falls through to _preRender_helper2
; ASMDOC-GEOMETRY Detail: Reads _sprite1 left/right-exclusive for sentinel pairs. For clipped top/bottom uses
; (left,left-1); for clipped left/right uses (right,right-1).
; ASMDOC-GEOMETRY Dead: false. Reachability: Live transitive internal calls from polygon rasterizer;
; asm/prerender_wheel_raster.ASM:299,305,338 (and dead line-record variant:1383,1407,1432)
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_preRender_helper label near
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender helper.
; ABI: NEAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: no named stack arguments referenced.
; Register ABI: touches AX, CX, DX, SI, DI, BP, CS; balanced explicit saves/restores observed: CX, DI; path-
; specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
preRender_helper proc near
    local_edge_cursor = word ptr -24
    mov     cx, [si+14h]
    or      cx, cx
    jle     short edge_clear_branch_path_01
    mov     di, [si+6]
    mov     dx, [si+4]
    add     dx, 8000h
    adc     di, 0
    sub     di, cx
    shl     di, 1
    add     di, [bp+local_edge_cursor]
    mov     ax, cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    push    cx
    push    di
    rep stosw
    pop     di
    pop     cx
    add     di, 3C0h
    dec     ax
    rep stosw
edge_clear_branch_path_01:
    mov     cx, [si+18h]
    or      cx, cx
    jle     short edge_clear_branch_path_02
    mov     di, [si+6]
    mov     dx, [si+4]
    add     dx, 8000h
    adc     di, 0
    sub     di, cx
    shl     di, 1
    add     di, [bp+local_edge_cursor]
    mov     ax, cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    push    cx
    push    di
    rep stosw
    pop     di
    pop     cx
    add     di, 3C0h
    dec     ax
    rep stosw
edge_clear_branch_path_02:
    mov     cx, [si+16h]
    or      cx, cx
    jle     short edge_clear_branch_path_03
    mov     di, [si+0Ah]
    inc     di
    shl     di, 1
    add     di, [bp+local_edge_cursor]
    mov     ax, cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    push    cx
    push    di
    rep stosw
    pop     di
    pop     cx
    add     di, 3C0h
    dec     ax
    rep stosw
edge_clear_branch_path_03:
    mov     cx, [si+1Ah]
    or      cx, cx
    jle     short preRender_helper2
    mov     di, [si+0Ah]
    inc     di
    shl     di, 1
    add     di, [bp+local_edge_cursor]
    mov     ax, cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    push    cx
    push    di
    rep stosw
    pop     di
    pop     cx
    add     di, 3C0h
    dec     ax
    rep stosw
preRender_helper endp
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _preRender_helper2 (original line 463)
; ASMDOC-GEOMETRY Purpose: Seed both scanline boundary arrays from one line record using the mode dispatch
; table.
; ASMDOC-GEOMETRY Proposed public: _preRender_helper2
; ASMDOC-GEOMETRY Convention: NEAR register worker; BP is inherited outer frame, not worker frame; RETN
; returns to internal caller
; ASMDOC-GEOMETRY Input: DS:SI: 28-byte prepared line record: +0 x fraction u16,+2 start x i16,+4 y fraction
; u16,+6 start y i16,+8 end x i16,+10 end y i16,+12 slope u16,+14 sample count i16,+16 color,+18 mode byte
; (0..9),+19 clip flags,+20/+22 top/bottom rows,+24/+26 left/right rows
; ASMDOC-GEOMETRY Input: SS:BP-24 word: base near offset of left scanline array; right array is base+0x3C0;
; absolute y indexes into these arrays
; ASMDOC-GEOMETRY Input: ES=DS=SS, DF=0; BP remains outer polygon frame; NEAR CALL supplies only a return IP,
; no stack arguments
; ASMDOC-GEOMETRY Output: Updates ES scanline arrays; no scalar return; line record and SI unchanged
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: ES
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: DI
; ASMDOC-GEOMETRY Clobbers: FLAGS
; ASMDOC-GEOMETRY Detail: Mode 0,1,9 or nonpositive sample count returns immediately. Modes 2..8 implement
; vertical, unit-slope, y-major and x-major fixed-point stepping; rounding adds 0x8000 with carry.
; ASMDOC-GEOMETRY Dead: false. Reachability: Live transitive internal calls from polygon rasterizer;
; asm/prerender_wheel_raster.ASM:299,305,338 (and dead line-record variant:1383,1407,1432)
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_preRender_helper2 label near
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender helper2.
; ABI: NEAR entry; source return form: RET mnemonic for NEAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, CS; balanced explicit saves/restores observed: none
; inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
preRender_helper2 proc near
    local_edge_cursor = word ptr -24
    mov     cx, [si+0Eh]
    or      cx, cx
    jle     short slope_fill_mode_case_02
    mov     di, [si+6]
    shl     di, 1
    add     di, [bp+local_edge_cursor]
    mov     bl, [si+12h]
    xor     bh, bh
    shl     bx, 1
    jmp     cs:sprite_line_fill_mode_dispatch[bx]
; ASMDOC-GEOMETRY Code-resident 10-word jump table indexed by unsigned mode byte; legal modes 0..9 are
; required. Entries 0/1/9 go to RET, not hardware instructions.
sprite_line_fill_mode_dispatch     dw offset slope_fill_mode_case_02
    dw offset slope_fill_mode_case_02
    dw offset slope_fill_mode_case_01
    dw offset slope_fill_mode_case_03
    dw offset slope_fill_mode_case_04
    dw offset slope_fill_mode_case_05
    dw offset slope_fill_mode_case_06
    dw offset slope_fill_mode_case_07
    dw offset slope_fill_mode_case_08
    dw offset slope_fill_mode_case_02
slope_fill_mode_case_01:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_fill_loop_head_01:
    mov     [di+3C0h], ax
    stosw
    loop    slope_fill_loop_head_01
slope_fill_mode_case_02:
    retn
slope_fill_mode_case_03:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_fill_loop_head_02:
    mov     [di+3C0h], ax
    stosw
    dec     ax
    loop    slope_fill_loop_head_02
    retn
slope_fill_mode_case_04:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_fill_loop_head_03:
    mov     [di+3C0h], ax
    stosw
    inc     ax
    loop    slope_fill_loop_head_03
    retn
slope_fill_mode_case_05:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
slope_fill_loop_head_04:
    mov     [di+3C0h], ax
    stosw
    sub     dx, bx
    sbb     ax, 0
    loop    slope_fill_loop_head_04
    retn
slope_fill_mode_case_06:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
slope_fill_loop_head_05:
    mov     [di+3C0h], ax
    stosw
    add     dx, bx
    adc     ax, 0
    loop    slope_fill_loop_head_05
    retn
slope_fill_mode_case_07:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short slope_fill_loop_head_06
    add     di, 2
slope_fill_loop_head_06:
    mov     [di+3C0h], ax
slope_fill_loop_head_07:
    add     dx, bx
    jnb     short slope_fill_branch_path_01
    stosw
    dec     ax
    loop    slope_fill_loop_head_06
    sub     di, 2
    retn
slope_fill_branch_path_01:
    dec     ax
    loop    slope_fill_loop_head_07
    inc     ax
    mov     [di], ax
    retn
slope_fill_mode_case_08:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short slope_fill_loop_head_08
    add     di, 2
slope_fill_loop_head_08:
    mov     [di], ax
slope_fill_loop_head_09:
    add     dx, bx
    jnb     short slope_fill_branch_path_02
    mov     [di+3C0h], ax
    add     di, 2
    inc     ax
    loop    slope_fill_loop_head_08
    sub     di, 2
    retn
slope_fill_branch_path_02:
    inc     ax
    loop    slope_fill_loop_head_09
    dec     ax
    mov     [di+3C0h], ax
    retn
preRender_helper2 endp
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _preRender_helper3 (original line 599)
; ASMDOC-GEOMETRY Purpose: Merge the second polygon chain into existing scanline boundaries and restore
; clipped boundary rows.
; ASMDOC-GEOMETRY Proposed public: _preRender_helper3
; ASMDOC-GEOMETRY Convention: NEAR register worker; BP is inherited outer frame, not worker frame; RETN
; returns to internal caller
; ASMDOC-GEOMETRY Input: DS:SI: 28-byte prepared line record: +0 x fraction u16,+2 start x i16,+4 y fraction
; u16,+6 start y i16,+8 end x i16,+10 end y i16,+12 slope u16,+14 sample count i16,+16 color,+18 mode byte
; (0..9),+19 clip flags,+20/+22 top/bottom rows,+24/+26 left/right rows
; ASMDOC-GEOMETRY Input: SS:BP-24 word: base near offset of left scanline array; right array is base+0x3C0;
; absolute y indexes into these arrays
; ASMDOC-GEOMETRY Input: ES=DS=SS, DF=0; BP remains outer polygon frame; NEAR CALL supplies only a return IP,
; no stack arguments
; ASMDOC-GEOMETRY Input: byte [SS:BP-10] merge selector; byte [SS:BP-12] clipping flag
; ASMDOC-GEOMETRY Output: Updates ES scanline arrays; no scalar return; line record and SI unchanged
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: ES
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: DI
; ASMDOC-GEOMETRY Clobbers: FLAGS
; ASMDOC-GEOMETRY Global read: _sprite1 +26/+28 clip edges
; ASMDOC-GEOMETRY Detail: SS:BP-10 byte selects per-row extrema merge (nonzero) or choose-a-side run fill
; (zero); SS:BP-12 byte enables clip postamble.
; ASMDOC-GEOMETRY Detail: Mode 0,1,9 dispatch directly to shared RET in helper2 and bypass the clip postamble.
; ASMDOC-GEOMETRY Detail: For top/bottom clipped rows postamble writes left only; for left/right clipped rows
; it writes right=clip_right-1 only.
; ASMDOC-GEOMETRY Dead: false. Reachability: Live transitive internal calls from polygon rasterizer;
; asm/prerender_wheel_raster.ASM:299,305,338 (and dead line-record variant:1383,1407,1432)
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_preRender_helper3 label near
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender helper3.
; ABI: NEAR entry; source return form: RET mnemonic for NEAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, CS; balanced explicit saves/restores observed: none
; inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
preRender_helper3 proc near
    local_edge_cursor = word ptr -24
    local_clip_required = byte ptr -12
    local_merge_mode = byte ptr -10
    mov     cx, [si+0Eh]
    or      cx, cx
    jle     short slope_raster_branch_path_02
    mov     di, [si+6]
    shl     di, 1
    add     di, [bp+local_edge_cursor]
    mov     bl, [si+12h]
    xor     bh, bh
    shl     bx, 1
    cmp     [bp+local_merge_mode], 0
    jnz     short slope_raster_branch_path_01
    jmp     cs:sprite_line_merge_mode_dispatch[bx]
slope_raster_branch_path_01:
    jmp     cs:sprite_line_merge_mode_alt[bx]
; ASMDOC-GEOMETRY Nonzero merge selector: compare each raster sample with current signed left/right extrema.
; This is code data.
sprite_line_merge_mode_alt     dw offset slope_fill_mode_case_02
    dw offset slope_fill_mode_case_02
    dw offset slope_raster_mode_case_01
    dw offset slope_raster_mode_case_02
    dw offset slope_raster_mode_case_03
    dw offset slope_raster_mode_case_04
    dw offset slope_raster_mode_case_05
    dw offset slope_raster_mode_case_06
    dw offset slope_raster_mode_case_07
    dw offset slope_fill_mode_case_02
slope_raster_mode_case_01:
    mov     ax, [si+2]
slope_raster_loop_head_01:
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_03
    cmp     [di], ax
    jle     short slope_raster_branch_path_04
    stosw
    loop    slope_raster_loop_head_01
slope_raster_branch_path_02:
    jmp     slope_raster_join_01
slope_raster_branch_path_03:
    mov     [di+3C0h], ax
slope_raster_branch_path_04:
    add     di, 2
    loop    slope_raster_loop_head_01
    jmp     slope_raster_join_01
slope_raster_mode_case_02:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_raster_loop_head_02:
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_05
    cmp     [di], ax
    jle     short slope_raster_branch_path_06
    stosw
    dec     ax
    loop    slope_raster_loop_head_02
    jmp     slope_raster_join_01
slope_raster_branch_path_05:
    mov     [di+3C0h], ax
slope_raster_branch_path_06:
    add     di, 2
    dec     ax
    loop    slope_raster_loop_head_02
    jmp     slope_raster_join_01
slope_raster_mode_case_03:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_raster_loop_head_03:
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_07
    cmp     [di], ax
    jle     short slope_raster_branch_path_08
    stosw
    inc     ax
    loop    slope_raster_loop_head_03
    jmp     slope_raster_join_01
slope_raster_branch_path_07:
    mov     [di+3C0h], ax
slope_raster_branch_path_08:
    add     di, 2
    inc     ax
    loop    slope_raster_loop_head_03
    jmp     slope_raster_join_01
slope_raster_mode_case_04:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
slope_raster_loop_head_04:
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_09
    cmp     [di], ax
    jle     short slope_raster_branch_path_10
    stosw
    sub     dx, bx
    sbb     ax, 0
    loop    slope_raster_loop_head_04
    jmp     slope_raster_join_01
slope_raster_branch_path_09:
    mov     [di+3C0h], ax
slope_raster_branch_path_10:
    add     di, 2
    sub     dx, bx
    sbb     ax, 0
    loop    slope_raster_loop_head_04
    jmp     slope_raster_join_01
slope_raster_mode_case_05:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
slope_raster_loop_head_05:
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_11
    cmp     [di], ax
    jle     short slope_raster_branch_path_12
    stosw
    add     dx, bx
    adc     ax, 0
    loop    slope_raster_loop_head_05
    jmp     slope_raster_join_01
slope_raster_branch_path_11:
    mov     [di+3C0h], ax
slope_raster_branch_path_12:
    add     di, 2
    add     dx, bx
    adc     ax, 0
    loop    slope_raster_loop_head_05
    jmp     slope_raster_join_01
slope_raster_mode_case_06:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short slope_raster_loop_head_06
    add     di, 2
slope_raster_loop_head_06:
    cmp     [di+3C0h], ax
    jge     short slope_raster_loop_head_07
    mov     [di+3C0h], ax
slope_raster_loop_head_07:
    add     dx, bx
    jnb     short slope_raster_branch_path_14
    cmp     [di], ax
    jle     short slope_raster_branch_path_13
    mov     [di], ax
slope_raster_branch_path_13:
    add     di, 2
    dec     ax
    loop    slope_raster_loop_head_06
    sub     di, 2
    jmp     slope_raster_join_01
slope_raster_branch_path_14:
    dec     ax
    loop    slope_raster_loop_head_07
    inc     ax
    cmp     [di], ax
    jle     short slope_raster_loop_head_08
    mov     [di], ax
slope_raster_loop_head_08:
    jmp     slope_raster_join_01
slope_raster_mode_case_07:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short slope_raster_loop_head_09
    add     di, 2
slope_raster_loop_head_09:
    cmp     [di], ax
    jle     short slope_raster_loop_head_10
    mov     [di], ax
slope_raster_loop_head_10:
    add     dx, bx
    jnb     short slope_raster_branch_path_16
    cmp     [di+3C0h], ax
    jge     short slope_raster_branch_path_15
    mov     [di+3C0h], ax
slope_raster_branch_path_15:
    add     di, 2
    inc     ax
    loop    slope_raster_loop_head_09
    sub     di, 2
    jmp     slope_raster_join_01
slope_raster_branch_path_16:
    inc     ax
    loop    slope_raster_loop_head_10
    dec     ax
    cmp     [di+3C0h], ax
    jge     short slope_raster_loop_head_08
    mov     [di+3C0h], ax
    jmp     slope_raster_join_01
; ASMDOC-GEOMETRY Zero selector: scan until the edge escapes current span, then fill that one boundary for the
; remaining run. This is code data.
sprite_line_merge_mode_dispatch     dw offset slope_fill_mode_case_02
    dw offset slope_fill_mode_case_02
    dw offset slope_raster_mode_case_08
    dw offset slope_raster_mode_case_09
    dw offset slope_raster_mode_case_10
    dw offset slope_raster_mode_case_11
    dw offset slope_raster_mode_case_12
    dw offset slope_raster_mode_case_13
    dw offset slope_raster_mode_case_14
    dw offset slope_fill_mode_case_02
slope_raster_mode_case_08:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_raster_loop_head_11:
    cmp     [di], ax
    jg      short slope_raster_branch_path_17
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_18
    add     di, 2
    loop    slope_raster_loop_head_11
    jmp     slope_raster_join_01
slope_raster_branch_path_17:
    rep stosw
    jmp     slope_raster_join_01
slope_raster_branch_path_18:
    add     di, 3C0h
    rep stosw
    jmp     slope_raster_join_01
slope_raster_mode_case_09:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_raster_loop_head_12:
    cmp     [di], ax
    jg      short slope_raster_loop_head_13
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_19
    add     di, 2
    dec     ax
    loop    slope_raster_loop_head_12
    jmp     slope_raster_join_01
slope_raster_loop_head_13:
    stosw
    dec     ax
    loop    slope_raster_loop_head_13
    jmp     slope_raster_join_01
slope_raster_branch_path_19:
    add     di, 3C0h
slope_raster_loop_head_14:
    stosw
    dec     ax
    loop    slope_raster_loop_head_14
    jmp     slope_raster_join_01
slope_raster_mode_case_10:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_raster_loop_head_15:
    cmp     [di], ax
    jg      short slope_raster_loop_head_16
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_20
    add     di, 2
    inc     ax
    loop    slope_raster_loop_head_15
    jmp     slope_raster_join_01
slope_raster_loop_head_16:
    stosw
    inc     ax
    loop    slope_raster_loop_head_16
    jmp     slope_raster_join_01
slope_raster_branch_path_20:
    add     di, 3C0h
slope_raster_loop_head_17:
    stosw
    inc     ax
    loop    slope_raster_loop_head_17
    jmp     slope_raster_join_01
slope_raster_mode_case_11:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
slope_raster_loop_head_18:
    cmp     [di], ax
    jg      short slope_raster_loop_head_19
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_21
    add     di, 2
    sub     dx, bx
    sbb     ax, 0
    loop    slope_raster_loop_head_18
    jmp     slope_raster_join_01
slope_raster_loop_head_19:
    stosw
    sub     dx, bx
    sbb     ax, 0
    loop    slope_raster_loop_head_19
    jmp     slope_raster_join_01
slope_raster_branch_path_21:
    add     di, 3C0h
slope_raster_loop_head_20:
    stosw
    sub     dx, bx
    sbb     ax, 0
    loop    slope_raster_loop_head_20
    jmp     slope_raster_join_01
slope_raster_mode_case_12:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
slope_raster_loop_head_21:
    cmp     [di], ax
    jg      short slope_raster_loop_head_22
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_22
    add     di, 2
    add     dx, bx
    adc     ax, 0
    loop    slope_raster_loop_head_21
    jmp     slope_raster_join_01
slope_raster_loop_head_22:
    stosw
    add     dx, bx
    adc     ax, 0
    loop    slope_raster_loop_head_22
    jmp     slope_raster_join_01
slope_raster_branch_path_22:
    add     di, 3C0h
slope_raster_loop_head_23:
    stosw
    add     dx, bx
    adc     ax, 0
    loop    slope_raster_loop_head_23
    jmp     slope_raster_join_01
slope_raster_mode_case_13:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short slope_raster_loop_head_24
    add     di, 2
slope_raster_loop_head_24:
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_24
    add     dx, bx
    jnb     short slope_raster_branch_path_23
    cmp     [di], ax
    jg      short slope_raster_branch_path_26
    add     di, 2
slope_raster_branch_path_23:
    dec     ax
    loop    slope_raster_loop_head_24
    jmp     slope_raster_join_01
slope_raster_branch_path_24:
    add     di, 3C0h
slope_raster_loop_head_25:
    stosw
slope_raster_loop_head_26:
    dec     ax
    add     dx, bx
    jnb     short slope_raster_branch_path_25
    loop    slope_raster_loop_head_25
    jmp     short slope_raster_join_01
    even ; word-align the following entry
slope_raster_branch_path_25:
    loop    slope_raster_loop_head_26
    jmp     short slope_raster_join_01
    nop ; retained code pad at an even offset; EVEN would emit no byte
slope_raster_loop_head_27:
    add     dx, bx
    jnb     short slope_raster_branch_path_27
slope_raster_branch_path_26:
    stosw
    dec     ax
    loop    slope_raster_loop_head_27
    jmp     short slope_raster_join_01
    even ; word-align the following entry
slope_raster_branch_path_27:
    dec     ax
    loop    slope_raster_loop_head_27
    inc     ax
    mov     [di], ax
    jmp     short slope_raster_join_01
    nop ; retained code pad at an even offset; EVEN would emit no byte
slope_raster_mode_case_14:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short slope_raster_loop_head_28
    add     di, 2
slope_raster_loop_head_28:
    cmp     [di], ax
    jg      short slope_raster_branch_path_31
    add     dx, bx
    jnb     short slope_raster_branch_path_28
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_29
    add     di, 2
slope_raster_branch_path_28:
    inc     ax
    loop    slope_raster_loop_head_28
    jmp     short slope_raster_join_01
    nop ; retained code pad at an even offset; EVEN would emit no byte
slope_raster_branch_path_29:
    add     di, 3C0h
slope_raster_loop_head_29:
    mov     [di], ax
slope_raster_loop_head_30:
    inc     ax
    add     dx, bx
    jnb     short slope_raster_branch_path_30
    add     di, 2
    loop    slope_raster_loop_head_29
    dec     ax
    mov     [di], ax
    jmp     short slope_raster_join_01
    nop ; retained code pad at an even offset; EVEN would emit no byte
slope_raster_branch_path_30:
    loop    slope_raster_loop_head_30
    add     di, 2
    dec     ax
    mov     [di], ax
    jmp     short slope_raster_join_01
    even ; word-align the following entry
slope_raster_loop_head_31:
    add     dx, bx
    jnb     short slope_raster_branch_path_32
slope_raster_branch_path_31:
    stosw
slope_raster_branch_path_32:
    inc     ax
    loop    slope_raster_loop_head_31
    jmp     short slope_raster_join_01
    nop ; retained code pad at an even offset; EVEN would emit no byte
; ASMDOC-GEOMETRY Clipping postamble differs from prefill: it rewrites only one array per clipped side.
; Dispatch-to-RET modes bypass this postamble.
slope_raster_join_01:
    cmp     [bp+local_clip_required], 0
    jnz     short slope_raster_branch_path_33
    retn
slope_raster_branch_path_33:
    mov     cx, [si+14h]
    or      cx, cx
    jle     short slope_raster_branch_path_34
    mov     di, [si+6]
    mov     dx, [si+4]
    add     dx, 8000h
    adc     di, 0
    sub     di, cx
    shl     di, 1
    add     di, [bp+local_edge_cursor]
    mov     ax, cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    rep stosw
slope_raster_branch_path_34:
    mov     cx, [si+18h]
    or      cx, cx
    jle     short slope_raster_branch_path_35
    mov     di, [si+6]
    mov     dx, [si+4]
    add     dx, 8000h
    adc     di, 0
    sub     di, cx
    shl     di, 1
    add     di, [bp+local_edge_cursor]
    add     di, 3C0h
    mov     ax, cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    dec     ax
    rep stosw
slope_raster_branch_path_35:
    mov     cx, [si+16h]
    or      cx, cx
    jle     short slope_raster_branch_path_36
    mov     di, [si+0Ah]
    inc     di
    shl     di, 1
    add     di, [bp+local_edge_cursor]
    mov     ax, cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    rep stosw
slope_raster_branch_path_36:
    mov     cx, [si+1Ah]
    or      cx, cx
    jle     short slope_raster_return_01
    mov     di, [si+0Ah]
    inc     di
    shl     di, 1
    add     di, [bp+local_edge_cursor]
    add     di, 3C0h
    mov     ax, cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    dec     ax
    rep stosw
slope_raster_return_01:
    retn
preRender_helper3 endp
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _nopsub_31F39 (original line 1104)
; ASMDOC-GEOMETRY Purpose: Rasterize a cyclic list of near pointers to already prepared 28-byte line records;
; merge mode 1.
; ASMDOC-GEOMETRY Proposed public: _sprite_fill_line_records
; ASMDOC-GEOMETRY Convention: FAR cdecl shared-frame wrapper; caller stack BP+6/+8/+10; RETF through sibling
; body
; ASMDOC-GEOMETRY Input: BP+6 word: span color
; ASMDOC-GEOMETRY Input: BP+8 word: positive record count; no guard
; ASMDOC-GEOMETRY Input: BP+10 onward: count inline word DS offsets to line records (not POINT2D coordinates)
; ASMDOC-GEOMETRY Input: DS=SS=DGROUP; each record layout is the same as the NEAR workers; ring must reach
; selected top and bottom records
; ASMDOC-GEOMETRY Output: Writes sprite1 pixels through span callback; may call _putpixel_line1_maybe at
; S012_TEXT:49A0 for count=1/degenerate input
; ASMDOC-GEOMETRY Output: Count=1 literally stores color to DS:[BP+DI+6], where DI is first record pointer;
; this is not DS:[DI+16]
; ASMDOC-GEOMETRY Output: Allocates same 0x7D0 scratch frame; no scalar result; DF cleared; ES changed
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: ES
; ASMDOC-GEOMETRY Clobbers: FLAGS (DF cleared)
; ASMDOC-GEOMETRY Global read: _sprite1 clip words +26 left,+28 right-exclusive,+16 top,+18 bottom-exclusive
; ASMDOC-GEOMETRY Global read: _spritefunc and _imagefunc far callback vectors
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global write: _spritefunc.offset = OFFSET _draw_filled_lines
; ASMDOC-GEOMETRY Global write: _imagefunc.offset = 49A0h; initialized segment remains S012_TEXT
; ASMDOC-GEOMETRY Global write: current sprite1 pixel backing; count=1 anomalous DS:[BP+DI+6] store
; ASMDOC-GEOMETRY Service/call: FAR [_imagefunc] = S012_TEXT:49A0 = _putpixel_line1_maybe(record*)
; (evidence/functions.json segment_offset=18848)
; ASMDOC-GEOMETRY Service/call: _preRender_helper / _preRender_helper2 / _preRender_helper3 NEAR
; ASMDOC-GEOMETRY Service/call: FAR [_spritefunc] _draw_filled_lines
; ASMDOC-GEOMETRY Detail: The two variants converge at line_raster_join_01; _nopsub_31F39 needs the later body
; for all exits.
; ASMDOC-GEOMETRY Detail: Mode lookup tables target worker case labels, not these wrappers; indirect callbacks
; go to selected image/span routines, not these wrappers.
; ASMDOC-GEOMETRY Detail: Literal callback binding is grounded by evidence/functions.json:
; putpixel_line1_maybe start=144320, segment_paragraph=7842, segment_offset=18848 (49A0h); source uses a
; numeric offset rather than a symbol.
; ASMDOC-GEOMETRY Dead: true. Reachability: No external symbolic call/address-taking or accepted source caller
; found; no dispatch table stores these entries. Name-plan dead=true. This is a static conclusion, not a
; runtime coverage proof.
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_sprite_fill_line_records label far
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 31F39.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (byte ptr 10).
; Register ABI: touches AX, SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
sprite_fill_line_records proc far
    local_line_descriptor = word ptr -2000
    local_left_edges = byte ptr -1944
    local_right_edges = byte ptr -984
    local_edge_cursor = word ptr -24
    var_16 = word ptr -22
    local_bottom_entry = word ptr -20
    local_bottom_y = word ptr -18
    local_top_entry = word ptr -16
    local_top_y = word ptr -14
    local_clip_required = byte ptr -12
    local_merge_mode = byte ptr -10
    local_last_list_entry = word ptr -8
    local_first_list_entry = word ptr -6
    local_clip_right_inclusive = word ptr -4
    local_clip_left = word ptr -2
    arg_color = word ptr 6
    arg_record_count = word ptr 8
    arg_inline_records = byte ptr 10
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     [bp+local_merge_mode], 1
    mov     ax, offset draw_filled_lines
    mov     spritefunc, ax
    mov     ax, 49A0h
    mov word ptr imagefunc, ax
    jmp     short line_raster_join_01
    nop ; retained code pad at an even offset; EVEN would emit no byte
sprite_fill_line_records endp
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _nopsub_31F55 (original line 1146)
; ASMDOC-GEOMETRY Purpose: Rasterize a cyclic list of near pointers to already prepared 28-byte line records;
; merge mode 0.
; ASMDOC-GEOMETRY Proposed public: _sprite_fill_line_records_alt
; ASMDOC-GEOMETRY Convention: Declared LABEL/PROC NEAR, but implementation consumes a FAR return frame (BP+6
; arguments) and executes RETF; ordinary NEAR CALL is invalid
; ASMDOC-GEOMETRY Input: BP+6 word: span color
; ASMDOC-GEOMETRY Input: BP+8 word: positive record count; no guard
; ASMDOC-GEOMETRY Input: BP+10 onward: count inline word DS offsets to line records (not POINT2D coordinates)
; ASMDOC-GEOMETRY Input: DS=SS=DGROUP; each record layout is the same as the NEAR workers; ring must reach
; selected top and bottom records
; ASMDOC-GEOMETRY Output: Writes sprite1 pixels through span callback; may call _putpixel_line1_maybe at
; S012_TEXT:49A0 for count=1/degenerate input
; ASMDOC-GEOMETRY Output: Count=1 literally stores color to DS:[BP+DI+6], where DI is first record pointer;
; this is not DS:[DI+16]
; ASMDOC-GEOMETRY Output: Allocates same 0x7D0 scratch frame; no scalar result; DF cleared; ES changed
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: ES
; ASMDOC-GEOMETRY Clobbers: FLAGS (DF cleared)
; ASMDOC-GEOMETRY Global read: _sprite1 clip words +26 left,+28 right-exclusive,+16 top,+18 bottom-exclusive
; ASMDOC-GEOMETRY Global read: _spritefunc and _imagefunc far callback vectors
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global write: _spritefunc.offset = OFFSET _draw_filled_lines
; ASMDOC-GEOMETRY Global write: _imagefunc.offset = 49A0h; initialized segment remains S012_TEXT
; ASMDOC-GEOMETRY Global write: current sprite1 pixel backing; count=1 anomalous DS:[BP+DI+6] store
; ASMDOC-GEOMETRY Service/call: FAR [_imagefunc] = S012_TEXT:49A0 = _putpixel_line1_maybe(record*)
; (evidence/functions.json segment_offset=18848)
; ASMDOC-GEOMETRY Service/call: _preRender_helper / _preRender_helper2 / _preRender_helper3 NEAR
; ASMDOC-GEOMETRY Service/call: FAR [_spritefunc] _draw_filled_lines
; ASMDOC-GEOMETRY Detail: The two variants converge at line_raster_join_01; _nopsub_31F39 needs the later body
; for all exits.
; ASMDOC-GEOMETRY Detail: Mode lookup tables target worker case labels, not these wrappers; indirect callbacks
; go to selected image/span routines, not these wrappers.
; ASMDOC-GEOMETRY Detail: Literal callback binding is grounded by evidence/functions.json:
; putpixel_line1_maybe start=144320, segment_paragraph=7842, segment_offset=18848 (49A0h); source uses a
; numeric offset rather than a symbol.
; ASMDOC-GEOMETRY Dead: true. Reachability: No external symbolic call/address-taking or accepted source caller
; found; no dispatch table stores these entries. Name-plan dead=true. This is a static conclusion, not a
; runtime coverage proof.
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_sprite_fill_line_records_alt label near
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 31F55. Its internal flow uses line_raster_join_01,
; line_raster_branch_path_01.
; ABI: NEAR entry; source return form: RETF mnemonic for NEAR entry; stack arguments: arg_0 (word ptr 6);
; arg_2 (word ptr 8); arg_4 (byte ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, ES, SS; balanced explicit saves/restores observed:
; DI, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
sprite_fill_line_records_alt proc near
    local_merge_mode = byte ptr -10
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     [bp+local_merge_mode], 0
    mov     ax, offset draw_filled_lines
    mov     spritefunc, ax
    mov     ax, 49A0h
    mov word ptr imagefunc, ax
    local_line_descriptor = word ptr -2000
    local_left_edges = byte ptr -1944
    local_right_edges = byte ptr -984
    local_edge_cursor = word ptr -24
    var_16 = word ptr -22
    local_bottom_entry = word ptr -20
    local_bottom_y = word ptr -18
    local_top_entry = word ptr -16
    local_top_y = word ptr -14
    local_clip_required = byte ptr -12
    local_merge_mode = byte ptr -10
    local_last_list_entry = word ptr -8
    local_first_list_entry = word ptr -6
    local_clip_right_inclusive = word ptr -4
    local_clip_left = word ptr -2
    arg_color = word ptr 6
    arg_record_count = word ptr 8
    arg_inline_records = byte ptr 10
; ASMDOC-GEOMETRY Alternate input representation is an inline array of near record pointers; geometry is read
; from record fields, not x/y vertices.
line_raster_join_01:
    lea     si, [bp+arg_inline_records]
    mov     [bp+local_first_list_entry], si
    mov     cx, [bp+arg_record_count]
    mov     ax, cx
    dec     ax
    shl     ax, 1
    add     ax, si
    mov     [bp+local_last_list_entry], ax
    cld
    mov     ax, ss
    mov     es, ax
    lea     ax, [bp+local_left_edges]
    mov     [bp+local_edge_cursor], ax
    mov     ax, cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    mov     [bp+local_clip_left], ax
    mov     ax, cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    dec     ax
    mov     [bp+local_clip_right_inclusive], ax
    mov     di, [si]
    mov     ax, [di+6]
    sub     ax, [di+14h]
    sub     ax, [di+18h]
    mov     [bp+local_top_y], ax
    mov     ax, [di+0Ah]
    add     ax, [di+16h]
    add     ax, [di+1Ah]
    mov     [bp+local_bottom_y], ax
    mov     [bp+local_line_descriptor], 3
    mov     bx, [di+2]
    mov     dx, [di+8]
    cmp     bx, dx
    jle     short line_raster_branch_path_01
    xchg    bx, dx
line_raster_branch_path_01:
    mov     [bp+local_top_entry], si
    mov     [bp+local_bottom_entry], si
    add     si, 2
    dec     cx
    jnz     short line_raster_loop_head_01
    mov     ax, [bp+arg_color]
    mov     [bp+di+arg_color], ax
    push    di
    call    dword ptr imagefunc
    add     sp, 2
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_raster_loop_head_01:
    mov     di, [si]
    mov     ax, [di+6]
    sub     ax, [di+14h]
    sub     ax, [di+18h]
    cmp     ax, [bp+local_top_y]
    jg      short line_raster_branch_path_03
    jnz     short line_raster_branch_path_02
    test    [bp+local_line_descriptor], 01h
    jz      short line_raster_branch_path_04
line_raster_branch_path_02:
    mov     [bp+local_top_y], ax
    mov     [bp+local_top_entry], si
    or      [bp+local_line_descriptor], 01h
    jmp     short line_raster_branch_path_04
    even ; word-align the following entry
    local_line_descriptor = word ptr -2000
    local_left_edges = byte ptr -1944
    local_right_edges = byte ptr -984
    local_edge_cursor = word ptr -24
    var_16 = word ptr -22
    local_bottom_entry = word ptr -20
    local_bottom_y = word ptr -18
    local_top_entry = word ptr -16
    local_top_y = word ptr -14
    local_clip_required = byte ptr -12
    local_merge_mode = byte ptr -10
    local_last_list_entry = word ptr -8
    local_first_list_entry = word ptr -6
    local_clip_right_inclusive = word ptr -4
    local_clip_left = word ptr -2
    arg_color = word ptr 6
    arg_record_count = word ptr 8
    arg_inline_records = byte ptr 10
line_raster_branch_path_03:
    and     [bp+local_line_descriptor], 02h
line_raster_branch_path_04:
    mov     ax, [di+0Ah]
    add     ax, [di+16h]
    add     ax, [di+1Ah]
    cmp     ax, [bp+local_bottom_y]
    jl      short line_raster_branch_path_07
    jnz     short line_raster_branch_path_06
line_raster_branch_path_05:
    test    [bp+local_line_descriptor], 02h
    jnz     short line_raster_branch_path_08
line_raster_branch_path_06:
    mov     [bp+local_bottom_y], ax
    mov     [bp+local_bottom_entry], si
    or      [bp+local_line_descriptor], 02h
    jmp     short line_raster_branch_path_08
    nop ; retained code pad at an even offset; EVEN would emit no byte
    local_line_descriptor = word ptr -2000
    local_left_edges = byte ptr -1944
    local_right_edges = byte ptr -984
    local_edge_cursor = word ptr -24
    var_16 = word ptr -22
    local_bottom_entry = word ptr -20
    local_bottom_y = word ptr -18
    local_top_entry = word ptr -16
    local_top_y = word ptr -14
    local_clip_required = byte ptr -12
    local_merge_mode = byte ptr -10
    local_last_list_entry = word ptr -8
    local_first_list_entry = word ptr -6
    local_clip_right_inclusive = word ptr -4
    local_clip_left = word ptr -2
    arg_color = word ptr 6
    arg_record_count = word ptr 8
    arg_inline_records = byte ptr 10
line_raster_branch_path_07:
    and     [bp+local_line_descriptor], 01h
line_raster_branch_path_08:
    mov     ax, [di+2]
    cmp     ax, bx
    jge     short line_raster_branch_path_09
    mov     bx, ax
line_raster_branch_path_09:
    cmp     ax, dx
    jle     short line_raster_branch_path_10
    mov     dx, ax
line_raster_branch_path_10:
    mov     ax, [di+8]
    cmp     ax, bx
    jge     short line_raster_branch_path_11
    mov     bx, ax
line_raster_branch_path_11:
    cmp     ax, dx
    jle     short line_raster_branch_path_12
    mov     dx, ax
line_raster_branch_path_12:
    add     si, 2
    loop    line_raster_loop_head_01
    cmp     dx, [bp+local_clip_left]
    jl      short line_raster_branch_path_16
    cmp     bx, [bp+local_clip_right_inclusive]
    jge     short line_raster_branch_path_16
    mov     ax, [bp+local_bottom_y]
    cmp     ax, cs:[_sprite1+SPRITE_TOP_OFFSET]
    jl      short line_raster_branch_path_16
    mov     cx, [bp+local_top_y]
    cmp     cx, cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jge     short line_raster_branch_path_16
    mov     [bp+local_clip_required], 0
    cmp     dx, [bp+local_clip_right_inclusive]
    jg      short line_raster_branch_path_13
    cmp     bx, [bp+local_clip_left]
    jl      short line_raster_branch_path_13
    cmp     ax, cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jge     short line_raster_branch_path_13
    cmp     cx, cs:[_sprite1+SPRITE_TOP_OFFSET]
    jge     short line_raster_branch_path_14
line_raster_branch_path_13:
    mov     [bp+local_clip_required], 1
line_raster_branch_path_14:
    cmp     ax, cx
    jz      short line_raster_branch_path_15
    cmp     dx, bx
    jnz     short line_raster_branch_path_17
line_raster_branch_path_15:
    mov     si, [bp+local_first_list_entry]
    mov     di, [si]
    push    di
    call    dword ptr imagefunc
    add     sp, 2
line_raster_branch_path_16:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_raster_branch_path_17:
    mov     di, [bp+local_top_entry]
line_raster_loop_head_02:
    mov     si, [di]
    cmp     byte ptr [si+12h], 2
    jl      short line_raster_branch_path_19
    mov     [bp+var_16], di
    cmp     [bp+local_clip_required], 0
    jz      short line_raster_branch_path_18
    call near ptr preRender_helper
    jmp     short line_raster_join_02
    even ; word-align the following entry
    local_line_descriptor = word ptr -2000
    local_left_edges = byte ptr -1944
    local_right_edges = byte ptr -984
    local_edge_cursor = word ptr -24
    var_16 = word ptr -22
    local_bottom_entry = word ptr -20
    local_bottom_y = word ptr -18
    local_top_entry = word ptr -16
    local_top_y = word ptr -14
    local_clip_required = byte ptr -12
    local_merge_mode = byte ptr -10
    local_last_list_entry = word ptr -8
    local_first_list_entry = word ptr -6
    local_clip_right_inclusive = word ptr -4
    local_clip_left = word ptr -2
    arg_color = word ptr 6
    arg_record_count = word ptr 8
    arg_inline_records = byte ptr 10
line_raster_branch_path_18:
    call near ptr preRender_helper2
line_raster_join_02:
    mov     di, [bp+var_16]
line_raster_branch_path_19:
    cmp     di, [bp+local_bottom_entry]
    jz      short line_raster_branch_path_20
    add     di, 2
    cmp     di, [bp+local_last_list_entry]
    jbe     short line_raster_loop_head_02
    mov     di, [bp+local_first_list_entry]
    jmp     short line_raster_loop_head_02
line_raster_branch_path_20:
    mov     di, [bp+local_top_entry]
line_raster_loop_head_03:
    sub     di, 2
    cmp     di, [bp+local_first_list_entry]
    jnb     short line_raster_branch_path_21
    mov     di, [bp+local_last_list_entry]
line_raster_branch_path_21:
    cmp     di, [bp+local_bottom_entry]
    jz      short line_raster_branch_path_22
    mov     si, [di]
    cmp     byte ptr [si+12h], 2
    jl      short line_raster_loop_head_03
    mov     [bp+var_16], di
    call near ptr preRender_helper3
    mov     di, [bp+var_16]
    jmp     short line_raster_loop_head_03
line_raster_branch_path_22:
    mov     ax, [bp+local_bottom_y]
    cmp     ax, cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jl      short line_raster_branch_path_23
    mov     ax, cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    dec     ax
line_raster_branch_path_23:
    mov     bx, [bp+local_top_y]
    cmp     bx, cs:[_sprite1+SPRITE_TOP_OFFSET]
    jge     short line_raster_branch_path_24
    mov     bx, cs:[_sprite1+SPRITE_TOP_OFFSET]
line_raster_branch_path_24:
    sub     ax, bx
    jle     short line_raster_branch_path_25
    inc     ax
    push    [bp+arg_color]
    push    ax
    push    bx
    shl     bx, 1
    lea     ax, [bp+local_right_edges]
    add     ax, bx
    push    ax
    lea     ax, [bp+local_left_edges]
    add     ax, bx
    push    ax
    call    dword ptr spritefunc
    add     sp, 0Ah
line_raster_branch_path_25:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
sprite_fill_line_records_alt endp
S012_TEXT ends
end
