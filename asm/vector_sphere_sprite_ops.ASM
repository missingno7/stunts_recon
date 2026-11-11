; ASMDOC-GEOMETRY Module: vector_sphere_sprite_ops. Existing content name accurately covers the mixed vector,
; sphere and sprite primitives; no merge or split.
; ASMDOC-GEOMETRY Original source: asm/vector_sphere_sprite_ops.ASM; contracts below use ORIGINAL line
; references.
; ASMDOC-GEOMETRY Code exports: _nopsub_33006, _vector_op_unk, _preRender_sphere, _nopsub_3320E, _sprset1size,
; _video_clear_color
; ASMDOC-GEOMETRY All arguments/coordinates are 16-bit unless explicitly stated. BP offsets are after PUSH
; BP/MOV BP,SP.
; ASMDOC-GEOMETRY The detailed comments describe control flow including shared tails and transitive services;
; old generic comments are retained.
; ASMDOC-GEOMETRY 
.8086
INCLUDE include\platform_hw.inc
name asm012_143366
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
extrn _sprite1:byte
extrn _prerender_max_line_width:word
extrn _vector_op_base_z:word
extrn _draw_filled_lines:far
extrn _preRender_sphere_helper:far
extrn _putpixel_single_maybe:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _nopsub_33006
public _vector_op_unk
public _preRender_sphere
public _nopsub_3320E
public _sprset1size
public _video_clear_color
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 33006.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: no named stack arguments referenced.
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _vector_op_base_z.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _nopsub_33006 (original line 32)
; ASMDOC-GEOMETRY Purpose: Intersect vector segment with global base-Z plane.
; ASMDOC-GEOMETRY Proposed public: _vector_intersect_base_z
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: near DS first VECTOR pointer (6 bytes)
; ASMDOC-GEOMETRY Input: BP+8 word: near DS second VECTOR pointer (6 bytes)
; ASMDOC-GEOMETRY Input: BP+10 word: near DS output VECTOR pointer (6 bytes)
; ASMDOC-GEOMETRY Input: target_z from _vector_op_base_z; no fourth argument read
; ASMDOC-GEOMETRY Output: Writes output.z first, then reads both input z values; computes n=word(target_z-
; second.z), d=word(first.z-second.z)
; ASMDOC-GEOMETRY Output: If signed d<0, logically shifts BOTH n and d right by one; uses signed IMUL of
; wrapped input component difference by signed n, signed IDIV by resulting d, then word-adds second component
; ASMDOC-GEOMETRY Output: Stores output.x before reading input y; input/output overlap observes these stores;
; divide by zero or signed quotient overflow can fault; no scalar return
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: CX
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: ES
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: FLAGS
; ASMDOC-GEOMETRY Global read: _vector_op_base_z
; ASMDOC-GEOMETRY Detail: Shared join must preserve the output-Z-before-input-Z ordering. port/matrix.c
; explicitly follows this width and alias behavior.
; ASMDOC-GEOMETRY Dead: true. Reachability: No external symbolic calls/address-taking or table target found;
; rename plan dead=true; tail-jumps to live interpolation body.
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_nopsub_33006 proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 4
    push    si
    push    di
    mov     ax, ds:_vector_op_base_z
    jmp     short vector_op_join_01
    ; align 2
    db 144
_nopsub_33006 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the graphics or geometry helper identified by vector op unk.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12).
; Register ABI: touches AX, BX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _vector_op_unk (original line 53)
; ASMDOC-GEOMETRY Purpose: Intersect vector segment with an explicit Z plane.
; ASMDOC-GEOMETRY Proposed public: _vector_intersect_z_plane
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: near DS first VECTOR pointer (6 bytes)
; ASMDOC-GEOMETRY Input: BP+8 word: near DS second VECTOR pointer (6 bytes)
; ASMDOC-GEOMETRY Input: BP+10 word: near DS output VECTOR pointer (6 bytes)
; ASMDOC-GEOMETRY Input: BP+12 word: target Z plane
; ASMDOC-GEOMETRY Output: Writes output.z first, then reads both input z values; computes n=word(target_z-
; second.z), d=word(first.z-second.z)
; ASMDOC-GEOMETRY Output: If signed d<0, logically shifts BOTH n and d right by one; uses signed IMUL of
; wrapped input component difference by signed n, signed IDIV by resulting d, then word-adds second component
; ASMDOC-GEOMETRY Output: Stores output.x before reading input y; input/output overlap observes these stores;
; divide by zero or signed quotient overflow can fault; no scalar return
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: CX
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: ES
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: FLAGS
; ASMDOC-GEOMETRY Detail: Shared join must preserve the output-Z-before-input-Z ordering. port/matrix.c
; explicitly follows this width and alias behavior.
; ASMDOC-GEOMETRY Dead: false. Reachability: src/obj_seg001_complete.c:1154,1297;
; src/obj_seg006.c:382,399,441,444
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_vector_op_unk proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 4
    push    si
    push    di
    mov     ax, [bp+arg_6]
; ASMDOC-GEOMETRY Store output Z BEFORE reading either input Z; aliased vectors can change
; numerator/denominator. Negative wrapped denominator uses logical SHR on both differences.
vector_op_join_01:
    mov     si, [bp+arg_0]
    mov     bx, [bp+arg_2]
    mov     di, [bp+arg_4]
    mov     word ptr [di+4], ax
    sub     ax, word ptr [bx+4]
    mov     [bp+var_4], ax
    mov     ax, word ptr [si+4]
    sub     ax, word ptr [bx+4]
    or      ax, ax
    jge     short vector_op_branch_path_01
    shr     [bp+var_4], 1
    shr     ax, 1
vector_op_branch_path_01:
    mov     [bp+var_2], ax
    mov     ax, word ptr [si+0]
    sub     ax, word ptr [bx+0]
    imul    [bp+var_4]
    idiv    [bp+var_2]
    add     ax, word ptr [bx+0]
    mov     word ptr [di+0], ax
    mov     ax, word ptr [si+2]
    sub     ax, word ptr [bx+2]
    imul    [bp+var_4]
    idiv    [bp+var_2]
    add     ax, word ptr [bx+2]
    mov     word ptr [di+2], ax
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
_vector_op_unk endp
; ASMDOC-GEOMETRY Unexported and unreferenced setter-like fragment; no return/prologue cleanup before falling
; into sphere prologue. Not a normal independently callable setter.
vector_set_base_z:
    push    bp
    mov     bp, sp
    mov     ax, [bp+6]
    mov     ds:_vector_op_base_z, ax
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender sphere.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS; balanced explicit saves/restores observed: DI,
; SI; path-specific inputs and outputs follow the listed operands.
; Globals read: _prerender_max_line_width, _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _preRender_sphere (original line 114)
; ASMDOC-GEOMETRY Purpose: Draw size-derived ellipse/sphere silhouette using mirrored profile spans or a
; 32-vertex fallback.
; ASMDOC-GEOMETRY Proposed public: _preRender_sphere
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: signed center x
; ASMDOC-GEOMETRY Input: BP+8 word: signed center y
; ASMDOC-GEOMETRY Input: BP+10 word: size; arithmetic begins as raw u16 shifts and wrapping
; subtraction/addition
; ASMDOC-GEOMETRY Input: BP+12 word: color passed to pixel/span/ring callback
; ASMDOC-GEOMETRY Input: DS=SS for stack span/ring buffer near pointers; sprite1 clip edges stored in CS
; ASMDOC-GEOMETRY Output: d=word(size-(u16(size)>>2)+((u16(size)>>2)>>2)); signed d<=0 returns; d=1 plots
; center
; ASMDOC-GEOMETRY Output: For d>=2: vertical top=center_y-(d>>1), profile radius r=d-(d>>1); mirrored
; left/right arrays support d rows, clipped to sprite1
; ASMDOC-GEOMETRY Output: If r>=_prerender_max_line_width (initially 40), builds six-word axis descriptor and
; calls _preRender_sphere_helper
; ASMDOC-GEOMETRY Output: Otherwise reads byte half-width profile selected by DS:[3C58h+2*r]; clips x
; endpoints and mirrored y spans then calls _draw_filled_lines; no scalar result
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: ES through raster callees
; ASMDOC-GEOMETRY Clobbers: FLAGS
; ASMDOC-GEOMETRY Global read: _sprite1 clip edges +26,+28,+16,+18
; ASMDOC-GEOMETRY Global read: _prerender_max_line_width
; ASMDOC-GEOMETRY Global read: DS:[3C58h+2*r] near-pointer profile table and profile bytes
; (graphics_resource_runtime)
; ASMDOC-GEOMETRY Global read: _sprite1 FAR bitmap pointer and line-offset table through pixel/span callbacks
; ASMDOC-GEOMETRY Global write: current sprite1 pixel backing through callees
; ASMDOC-GEOMETRY Service/call: _putpixel_single_maybe FAR for d=1
; ASMDOC-GEOMETRY Service/call: _preRender_sphere_helper FAR for large profile
; ASMDOC-GEOMETRY Service/call: _draw_filled_lines FAR for small profile; transitively _preRender_default_alt
; ASMDOC-GEOMETRY Detail: 0x79A-byte local allocation plus saved SI/DI; left array BP-0x3DA and right
; BP-0x79A.
; ASMDOC-GEOMETRY Detail: Coarse horizontal cull uses r+(r>>2), while fallback axis x uses u16(size)>>1.
; ASMDOC-GEOMETRY Detail: Horizontal/vertical coordinate arithmetic wraps as 16-bit before signed tests.
; ASMDOC-GEOMETRY Detail: Small-profile out-of-horizontal-range branch increments top and removes TWO mirrored
; rows; if reached after prior array writes, falling out there returns without drawing accumulated rows.
; ASMDOC-GEOMETRY Dead: false. Reachability: src/obj_seg006.c:687 sphere primitive dispatcher
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_preRender_sphere proc far
    push bp
    mov bp, sp
    sub sp, 79Ah
    push si
    push di
    mov dx, word ptr [bp + 0Ah]
    mov ax, dx
    shr ax, 1
    shr ax, 1
    sub dx, ax
    shr ax, 1
    shr ax, 1
    add dx, ax
    or dx, dx
    jg sphere_render_branch_path_01
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
sphere_render_branch_path_01:
    mov word ptr [bp - 6], dx
    mov bx, dx
    shr dx, 1
    jne sphere_render_branch_path_02
    push word ptr [bp + 0Ch]
    push word ptr [bp + 8]
    push word ptr [bp + 6]
    call far ptr _putpixel_single_maybe
    add sp, 6
sphere_render_loop_head_01:
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
sphere_render_branch_path_02:
    sub bx, dx
    mov ax, word ptr cs:[_sprite1+26]
    mov word ptr [bp - 4], ax
    mov ax, word ptr cs:[_sprite1+28]
    dec ax
    mov word ptr [bp - 2], ax
    mov cx, word ptr [bp + 8]
    mov ax, cx
    sub ax, dx
    cmp ax, word ptr cs:[_sprite1+18]
    jge sphere_render_loop_head_01
    mov word ptr [bp - 14h], ax
    add cx, bx
    cmp cx, word ptr cs:[_sprite1+16]
    jle sphere_render_loop_head_01
    mov dx, bx
    mov ax, dx
    shr ax, 1
    shr ax, 1
    add dx, ax
    mov cx, word ptr [bp + 6]
    mov ax, cx
    sub ax, dx
    cmp ax, word ptr [bp - 2]
    jg sphere_render_loop_head_01
    add cx, dx
    cmp cx, word ptr [bp - 4]
    jl sphere_render_loop_head_01
    cmp bx, word ptr ds:[_prerender_max_line_width]
    jl sphere_render_branch_path_03
    mov ax, word ptr [bp + 6]
    mov word ptr [bp - 12h], ax
    mov word ptr [bp - 0Eh], ax
    mov dx, word ptr [bp + 0Ah]
    shr dx, 1
    add ax, dx
    mov word ptr [bp - 0Ah], ax
    mov ax, word ptr [bp + 8]
    mov word ptr [bp - 10h], ax
    mov word ptr [bp - 8], ax
    mov dx, word ptr [bp - 6]
    shr dx, 1
    add ax, dx
    mov word ptr [bp - 0Ch], ax
    mov dx, word ptr [bp + 0Ch]
    lea ax, [bp - 12h]
    add sp, 780h
    push dx
    push ax
    call far ptr _preRender_sphere_helper
    add sp, 4
    sub sp, 780h
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
; ASMDOC-GEOMETRY Profile pointer table is addressed by absolute near offset 3C58h+2*radius. No bounds guard
; if max-line-width global is changed beyond table coverage.
sphere_render_branch_path_03:
    shl bx, 1
    mov ax, word ptr [bx + 3C58h]
    mov word ptr [bp - 16h], ax
    mov ax, word ptr [bp - 6]
    mov word ptr [bp - 1Ah], ax
    dec ax
    shl ax, 1
    mov word ptr [bp - 18h], ax
    lea si, [bp - 3DAh]
    lea di, [bp - 79Ah]
sphere_render_loop_head_02:
    mov bx, word ptr [bp - 16h]
    mov dl, byte ptr [bx]
    inc word ptr [bp - 16h]
    xor dh, dh
    mov bx, word ptr [bp - 18h]
    mov ax, word ptr [bp + 6]
    sub ax, dx
    cmp ax, word ptr [bp - 2]
    jg sphere_render_branch_path_06
    cmp ax, word ptr [bp - 4]
    jge sphere_render_branch_path_04
    mov ax, word ptr [bp - 4]
sphere_render_branch_path_04:
    mov word ptr [si], ax
    mov word ptr [bx + si], ax
    mov ax, word ptr [bp + 6]
    add ax, dx
    cmp ax, word ptr [bp - 4]
    jl sphere_render_branch_path_06
    cmp ax, word ptr [bp - 2]
    jle sphere_render_branch_path_05
    mov ax, word ptr [bp - 2]
sphere_render_branch_path_05:
    mov word ptr [di], ax
    mov word ptr [bx + di], ax
    add si, 2
    add di, 2
    sub word ptr [bp - 18h], 4
    jge sphere_render_loop_head_02
    jmp sphere_render_join_01
; ASMDOC-GEOMETRY Reject one mirrored pair, advancing top/removing two rows; if the loop ends here the routine
; returns without drawing any previously stored pairs.
sphere_render_branch_path_06:
    inc word ptr [bp - 14h]
    sub word ptr [bp - 1Ah], 2
    sub word ptr [bp - 18h], 4
    jge sphere_render_loop_head_02
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
sphere_render_join_01:
    xor dx, dx
    mov ax, word ptr cs:[_sprite1+16]
    sub ax, word ptr [bp - 14h]
    jle sphere_render_branch_path_07
    sub word ptr [bp - 1Ah], ax
    shl ax, 1
    mov dx, ax
    mov ax, word ptr cs:[_sprite1+16]
    mov word ptr [bp - 14h], ax
sphere_render_branch_path_07:
    mov ax, word ptr [bp - 14h]
    add ax, word ptr [bp - 1Ah]
    sub ax, word ptr cs:[_sprite1+18]
    jle sphere_render_branch_path_08
    sub word ptr [bp - 1Ah], ax
sphere_render_branch_path_08:
    push word ptr [bp + 0Ch]
    push word ptr [bp - 1Ah]
    push word ptr [bp - 14h]
    lea ax, [bp - 79Ah]
    add ax, dx
    push ax
    lea ax, [bp - 3DAh]
    add ax, dx
    push ax
    call far ptr _draw_filled_lines
    add sp, 0Ah
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
_preRender_sphere endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 3320E. Its internal flow uses
; noop_entry2_branch_path_01.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12); arg_8
;   (word ptr 14); arg_A (word ptr 16).
; Register ABI: touches AX, SI, BP, SP, CS, DS; balanced explicit saves/restores observed: DS, SI; path-
; specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: _sprite1.
; -----------------------------------------------------------------------------
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _nopsub_3320E (original line 303)
; ASMDOC-GEOMETRY Purpose: Set four clipping bounds in a supplied sprite; mirror them into active sprite1 if
; bitmap SEGMENTS match.
; ASMDOC-GEOMETRY Proposed public: _sprite_set_window_bounds
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6/+8 words: FAR sprite pointer offset/segment
; ASMDOC-GEOMETRY Input: BP+10 left word; BP+12 right word; BP+14 top word; BP+16 bottom word
; ASMDOC-GEOMETRY Input: Comparison reads supplied sprite+2 bitmap segment and CS:_sprite1+2 only; bitmap
; offsets are not compared
; ASMDOC-GEOMETRY Output: Stores left at supplied +26 and +12; right at +28 and +14; top at +16; bottom at +18
; ASMDOC-GEOMETRY Output: If bitmap segments match, repeats exactly those stores in CS:_sprite1. Bounds are
; unvalidated and unclamped
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: BX
; ASMDOC-GEOMETRY Preserves: CX
; ASMDOC-GEOMETRY Preserves: DX
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: ES
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: FLAGS
; ASMDOC-GEOMETRY Global read: _sprite1+2 bitmap segment
; ASMDOC-GEOMETRY Global write: supplied sprite +12,+14,+16,+18,+26,+28
; ASMDOC-GEOMETRY Global write: _sprite1 same fields when segments match
; ASMDOC-GEOMETRY Detail: Matching the bitmap segment alone is weaker than matching the whole far pointer.
; ASMDOC-GEOMETRY Dead: true. Reachability: No external symbolic calls/address-taking or table target found;
; rename plan dead=true.
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_nopsub_3320E proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12
    arg_8 = word ptr 14
    arg_A = word ptr 16

    push    bp
    mov     bp, sp
    push    ds
    push    si
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+2]
    cmp     word ptr cs:[_sprite1+2], ax
    jnz     short noop_entry2_branch_path_01
    mov     ax, [bp+arg_4]
    mov     [si+1Ah], ax
    mov     cs:[_sprite1+26], ax
    mov     [si+0Ch], ax
    mov     cs:[_sprite1+12], ax
    mov     ax, [bp+arg_6]
    mov     [si+1Ch], ax
    mov     cs:[_sprite1+28], ax
    mov     [si+0Eh], ax
    mov     cs:[_sprite1+14], ax
    mov     ax, [bp+arg_8]
    mov     [si+10h], ax
    mov     cs:[_sprite1+16], ax
    mov     ax, [bp+arg_A]
    mov     [si+12h], ax
    mov     cs:[_sprite1+18], ax
    pop     si
    pop     ds
    pop     bp
    retf
; ASMDOC-GEOMETRY A bitmap SEGMENT comparison alone controls active-descriptor mirroring; offsets are
; deliberately not tested.
noop_entry2_branch_path_01:
    mov     ax, [bp+arg_4]
    mov     [si+1Ah], ax
    mov     [si+0Ch], ax
    mov     ax, [bp+arg_6]
    mov     [si+1Ch], ax
    mov     [si+0Eh], ax
    mov     ax, [bp+arg_8]
    mov     [si+10h], ax
    mov     ax, [bp+arg_A]
    mov     [si+12h], ax
    pop     si
    pop     ds
    pop     bp
    retf
_nopsub_3320E endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprset1size.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12).
; Register ABI: touches AX, BP, SP, CS; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _sprite1.
; -----------------------------------------------------------------------------
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _sprset1size (original line 365)
; ASMDOC-GEOMETRY Purpose: Assign active sprite1 clipping rectangle and duplicate horizontal limits.
; ASMDOC-GEOMETRY Proposed public: _sprset1size
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word left; BP+8 word right; BP+10 word top; BP+12 word bottom
; ASMDOC-GEOMETRY Input: All four values are raw word stores; no range or ordering validation
; ASMDOC-GEOMETRY Output: left -> CS:_sprite1+26,+12; right -> +28,+14; top -> +16; bottom -> +18; AX=bottom
; ASMDOC-GEOMETRY Preserves: BX
; ASMDOC-GEOMETRY Preserves: CX
; ASMDOC-GEOMETRY Preserves: DX
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: ES
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Global write: _sprite1+12,+14,+16,+18,+26,+28
; ASMDOC-GEOMETRY Dead: false. Reachability: Many src/obj_seg000.c callers, including
; :845,849,861,865,1017,1658; rectangle bounds are right/bottom exclusive in consumers.
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_sprset1size proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_0]
    mov     cs:[_sprite1+26], ax
    mov     cs:[_sprite1+12], ax
    mov     ax, [bp+arg_2]
    mov     cs:[_sprite1+28], ax
    mov     cs:[_sprite1+14], ax
    mov     ax, [bp+arg_4]
    mov     cs:[_sprite1+16], ax
    mov     ax, [bp+arg_6]
    mov     cs:[_sprite1+18], ax
    pop     bp
    retf
_sprset1size endp
; -----------------------------------------------------------------------------
; Purpose: Clear the active video page to the requested color.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, CX, DI, BP, SP, ES; balanced explicit saves/restores observed: DI; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): selects a video-memory segment. */
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _video_clear_color (original line 397)
; ASMDOC-GEOMETRY Purpose: Fill graphics segment with the supplied WORD using 64000 STOSW iterations and
; 16-bit DI wrap.
; ASMDOC-GEOMETRY Proposed public: _video_clear_color
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: repeated WORD pattern, not merely low-byte color; callers normally supply
; equal color bytes
; ASMDOC-GEOMETRY Input: DF is inherited (no CLD); normal forward fill requires DF=0
; ASMDOC-GEOMETRY Output: ES=A000h; CX=0 after REP; DI restored; AX=pattern; 128000 byte stores over segment-
; relative offsets wrapping after 64KiB
; ASMDOC-GEOMETRY Output: With DF=0 starting DI=0: full A000:0000..FFFF written once and 0000..F3FF again;
; hidden tail FA00..FFFF is written too
; ASMDOC-GEOMETRY Output: With DF=1 first store at 0 then descending even addresses; same 64000 words,
; different overlap ordering
; ASMDOC-GEOMETRY Preserves: BX
; ASMDOC-GEOMETRY Preserves: DX
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Preserves: FLAGS (MOV/PUSH/POP/REP STOSW do not change FLAGS)
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: ES
; ASMDOC-GEOMETRY Global write: A000:0000..FFFF word-pattern fill (16-bit offset wrap)
; ASMDOC-GEOMETRY Service/call: Direct graphics-segment ES:DI stores; no BIOS/DOS/port instruction
; ASMDOC-GEOMETRY Detail: Zero byte after RETF is padding, not an entrypoint. platform-boundary.md correctly
; documents 64000 word writes.
; ASMDOC-GEOMETRY Dead: false. Reachability: asm/video_set_mode_13h.ASM:34;
; asm/video_mode_restore_handlers.ASM:79; asm/input_keyboard_joystick_services.ASM:70
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_video_clear_color proc far
    push bp
    mov bp,sp
    push di
    mov ax,R5HW_VIDEO_GRAPHICS_SEGMENT ;/* PLATFORM(video): selects a video-memory segment. */
    mov es,ax
    mov ax,word ptr [bp+6]
    mov di,0
    mov cx,0FA00h
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    pop di
    pop bp
    retf
    db 0
_video_clear_color endp
S012_TEXT ends
end
