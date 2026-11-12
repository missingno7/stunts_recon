; ASMDOC-GEOMETRY Module: projection_vector_window. Name already identifies the mixed projection/vector/window
; contents; preserve one module.
; ASMDOC-GEOMETRY Original source: asm/project.ASM; contracts below use ORIGINAL line
; references.
; ASMDOC-GEOMETRY Code exports: _nopsub_322C0, _nopsub_322DF, _set_projection, _vector_to_point,
; _sprite_free_window
; ASMDOC-GEOMETRY All arguments/coordinates are 16-bit unless explicitly stated. BP offsets are after PUSH
; BP/MOV BP,SP.
; ASMDOC-GEOMETRY The detailed comments describe control flow including shared tails and transitive services;
; old generic comments are retained.
; ASMDOC-GEOMETRY Data export: Ten initialized projection i16 words:
; half_width=160,half_height=100,left/top=0,center_x=160,center_y=100,x/y scales=0,x/y angles=0
; ASMDOC-GEOMETRY Data export: _window_release_order_error_msg: zero-terminated bytes "Window Released Out of
; Order\r\n", followed by one padding zero
; ASMDOC-GEOMETRY 
.8086
INCLUDE include\platform_hw.inc
VECTOR struc
vx dw ?
vy dw ?
vz dw ?
VECTOR ends
S012_TEXT segment word public 'CODE'
S012_TEXT ends
DGROUP group _DATA, CONST, _BSS
assume ds:DGROUP
_DATA segment word public 'DATA'
public _projection_half_width
public _projection_half_height
public _projection_left_origin
public _projection_top_origin
public _projection_center_x
public _projection_center_y
public _projection_x_scale
public _projection_y_scale
public _projection_x_angle
public _projection_y_angle
public _window_release_order_error_msg
_projection_half_width     dw 160
_projection_half_height     dw 100
_projection_left_origin     dw 0
_projection_top_origin     dw 0
_projection_center_x     dw 160
_projection_center_y     dw 100
_projection_x_scale     dw 0
_projection_y_scale     dw 0
_projection_x_angle     dw 0
_projection_y_angle     dw 0
_window_release_order_error_msg db 'Window Released Out of Order', 0Dh, 0Ah, 00h, 00h
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
assume cs:S012_TEXT
extrn _intcos:far
extrn _intatan:far
extrn _intsin:far
extrn _purgememory:far
extrn _abortmessage:far
S012_TEXT segment word public 'CODE'
public _projectorgp
public _setprojectpb
public _set_projection
public _projectp
public _sprite_free_window
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 322C0.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _projection_half_height, _projection_half_width.
; Globals written: _projection_center_x, _projection_center_y, _projection_left_origin,
; _projection_top_origin.
; -----------------------------------------------------------------------------
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _nopsub_322C0 (original line 91)
; ASMDOC-GEOMETRY Purpose: Set projection viewport origin and recompute centers using the current half
; dimensions.
; ASMDOC-GEOMETRY Proposed public: _projection_set_origin
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: left origin
; ASMDOC-GEOMETRY Input: BP+8 word: top origin
; ASMDOC-GEOMETRY Output: Stores left/top and centers with 16-bit wrapping additions; AX=updated center_y
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
; ASMDOC-GEOMETRY Clobbers: FLAGS
; ASMDOC-GEOMETRY Global read: _projection_half_width
; ASMDOC-GEOMETRY Global read: _projection_half_height
; ASMDOC-GEOMETRY Global write: _projection_left_origin
; ASMDOC-GEOMETRY Global write: _projection_top_origin
; ASMDOC-GEOMETRY Global write: _projection_center_x
; ASMDOC-GEOMETRY Global write: _projection_center_y
; ASMDOC-GEOMETRY Dead: true. Reachability: No symbolic call/address-taking found outside owner; no
; callback/table target; rename plan dead=true.
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_projectorgp proc far
    arg_left_origin = word ptr 6
    arg_top_origin = word ptr 8

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_left_origin]
    mov     _projection_left_origin, ax
    add     ax, _projection_half_width
    mov     _projection_center_x, ax
    mov     ax, [bp+arg_top_origin]
    mov     _projection_top_origin, ax
    add     ax, _projection_half_height
    mov     _projection_center_y, ax
    pop     bp
    retf
_projectorgp endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 322DF.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: arg_0 (word ptr 6); arg_2 (word ptr 8).
; Register ABI: touches AX, SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _projection_x_angle, _projection_y_angle.
; -----------------------------------------------------------------------------
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _nopsub_322DF (original line 117)
; ASMDOC-GEOMETRY Purpose: Configure projection from angles already expressed in game angle units.
; ASMDOC-GEOMETRY Proposed public: _projection_set_angle_units
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: horizontal angle in game units (1024/turn); passed verbatim to trig
; ASMDOC-GEOMETRY Input: BP+8 word: vertical angle in game units; passed verbatim; zero selects derived
; vertical scale
; ASMDOC-GEOMETRY Input: BP+10 word: unsigned width; logical SHR 1 supplies half width
; ASMDOC-GEOMETRY Input: BP+12 word: unsigned height; logical SHR 1 supplies half height
; ASMDOC-GEOMETRY Output: Centers = origin + half extent modulo 65536
; ASMDOC-GEOMETRY Output: Scales use unsigned (cos(angle)*half_extent)/sin(angle); DIV may fault on zero sine
; or quotient overflow
; ASMDOC-GEOMETRY Output: If y angle=0: y_scale=x_scale-(x_scale>>3)-((x_scale>>3)>>1);
; y_angle=_polang(y_scale,half_height) with first stack argument y_scale
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Preserves: ES
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: FLAGS
; ASMDOC-GEOMETRY Global read: _projection_left_origin
; ASMDOC-GEOMETRY Global read: _projection_top_origin
; ASMDOC-GEOMETRY Global write: _projection_x_angle
; ASMDOC-GEOMETRY Global write: _projection_y_angle
; ASMDOC-GEOMETRY Global write: _projection_half_width
; ASMDOC-GEOMETRY Global write: _projection_half_height
; ASMDOC-GEOMETRY Global write: _projection_center_x
; ASMDOC-GEOMETRY Global write: _projection_center_y
; ASMDOC-GEOMETRY Global write: _projection_x_scale
; ASMDOC-GEOMETRY Global write: _projection_y_scale
; ASMDOC-GEOMETRY Service/call: _cosfast FAR
; ASMDOC-GEOMETRY Service/call: _sinfast FAR
; ASMDOC-GEOMETRY Service/call: _polang FAR only for zero y angle
; ASMDOC-GEOMETRY Detail: Shares prologue, four arguments and epilogue with set_projection; old comments
; listing two arguments are incomplete.
; ASMDOC-GEOMETRY Dead: true. Reachability: No external symbolic calls or address-taking found; jumps into
; set_projection at projection_join_01; rename plan dead=true.
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_setprojectpb proc far
    arg_horizontal_angle = word ptr 6
    arg_vertical_angle = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_horizontal_angle]
    mov     _projection_x_angle, ax
    mov     ax, [bp+arg_vertical_angle]
    mov     _projection_y_angle, ax
    jmp     short projection_join_01
    nop ; retained code pad at an even offset; EVEN would emit no byte
_setprojectpb endp
; -----------------------------------------------------------------------------
; Purpose: Set the projection parameters used by vector rendering.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _projection_half_height, _projection_half_width, _projection_left_origin,
; _projection_top_origin, _projection_x_angle, _projection_y_angle.
; Globals written: _projection_center_x, _projection_center_y, _projection_half_height,
; _projection_half_width, _projection_x_angle, _projection_x_scale, _projection_y_angle,
;   _projection_y_scale.
; -----------------------------------------------------------------------------
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _set_projection (original line 142)
; ASMDOC-GEOMETRY Purpose: Convert degree angles then configure projection scales and dimensions.
; ASMDOC-GEOMETRY Proposed public: _set_projection
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: horizontal angle in unsigned degrees
; ASMDOC-GEOMETRY Input: BP+8 word: vertical angle in unsigned degrees; zero selects derived vertical scale
; ASMDOC-GEOMETRY Input: BP+10 word: unsigned width; logical SHR 1 supplies half width
; ASMDOC-GEOMETRY Input: BP+12 word: unsigned height; logical SHR 1 supplies half height
; ASMDOC-GEOMETRY Output: Centers = origin + half extent modulo 65536
; ASMDOC-GEOMETRY Output: Scales use unsigned (cos(angle)*half_extent)/sin(angle); DIV may fault on zero sine
; or quotient overflow
; ASMDOC-GEOMETRY Output: If y angle=0: y_scale=x_scale-(x_scale>>3)-((x_scale>>3)>>1);
; y_angle=_polang(y_scale,half_height) with first stack argument y_scale
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Preserves: ES
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: FLAGS
; ASMDOC-GEOMETRY Global read: _projection_left_origin
; ASMDOC-GEOMETRY Global read: _projection_top_origin
; ASMDOC-GEOMETRY Global write: _projection_x_angle
; ASMDOC-GEOMETRY Global write: _projection_y_angle
; ASMDOC-GEOMETRY Global write: _projection_half_width
; ASMDOC-GEOMETRY Global write: _projection_half_height
; ASMDOC-GEOMETRY Global write: _projection_center_x
; ASMDOC-GEOMETRY Global write: _projection_center_y
; ASMDOC-GEOMETRY Global write: _projection_x_scale
; ASMDOC-GEOMETRY Global write: _projection_y_scale
; ASMDOC-GEOMETRY Service/call: _cosfast FAR
; ASMDOC-GEOMETRY Service/call: _sinfast FAR
; ASMDOC-GEOMETRY Service/call: _polang FAR only for zero y angle
; ASMDOC-GEOMETRY Detail: Degree conversion computes unsigned ((angle<<11)/360)>>1; DIV overflow occurs before
; the final right shift.
; ASMDOC-GEOMETRY Dead: false. Reachability: src/frontend.c:1013,1531; src/render.c:3094;
; src/race.c:820
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_set_projection proc far
    arg_horizontal_degrees = word ptr 6
    arg_vertical_degrees = word ptr 8
    arg_width = word ptr 10
    arg_height = word ptr 12

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_horizontal_degrees]
    xor     dh, dh
    mov     dl, ah
    mov     ah, al
    xor     al, al
    mov     cx, 3
projection_loop_head_01:
    shl     ax, 1
    rcl     dx, 1
    loop    projection_loop_head_01
    mov     cx, 168h
    div     cx
    shr     ax, 1
    mov     _projection_x_angle, ax
    mov     ax, [bp+arg_vertical_degrees]
    xor     dh, dh
    mov     dl, ah
    mov     ah, al
    xor     al, al
    mov     cx, 3
projection_loop_head_02:
    shl     ax, 1
    rcl     dx, 1
    loop    projection_loop_head_02
    mov     cx, 168h
    div     cx
    shr     ax, 1
    mov     _projection_y_angle, ax
; ASMDOC-GEOMETRY Both angle entry paths converge here; BOTH require width at BP+10 and height at BP+12.
; Direct-angle wrapper is not a two-argument function.
projection_join_01:
    mov     ax, [bp+arg_width]
    shr     ax, 1
    mov     _projection_half_width, ax
    add     ax, _projection_left_origin
    mov     _projection_center_x, ax
    mov     ax, [bp+arg_height]
    shr     ax, 1
    mov     _projection_half_height, ax
    add     ax, _projection_top_origin
    mov     _projection_center_y, ax
    push    _projection_x_angle
    call _intcos
    add     sp, 2
    mul     _projection_half_width
    mov     si, ax
    mov     di, dx
    push    _projection_x_angle
    call _intsin
    add     sp, 2
    mov     cx, ax
    mov     dx, di
    mov     ax, si
    div     cx
    mov     _projection_x_scale, ax
    mov     bx, _projection_y_angle
    or      bx, bx
    jz      short projection_branch_path_01
    push    _projection_y_angle
    call _intcos
    add     sp, 2
    mul     _projection_half_height
    mov     si, ax
    mov     di, dx
    push    _projection_y_angle
    call _intsin
    add     sp, 2
    mov     cx, ax
    mov     dx, di
    mov     ax, si
    div     cx
    mov     _projection_y_scale, ax
    pop     di
    pop     si
    pop     bp
    retf
; ASMDOC-GEOMETRY Zero vertical angle selects aspect-derived scale using two separate truncations, then polang
; receives (scale,half_height).
projection_branch_path_01:
    mov     bx, ax
    shr     bx, 1
    shr     bx, 1
    shr     bx, 1
    sub     ax, bx
    shr     bx, 1
    sub     ax, bx
    mov     _projection_y_scale, ax
    push    _projection_half_height
    push    ax
    call _intatan
    add     sp, 4
    mov     _projection_y_angle, ax
    pop     di
    pop     si
    pop     bp
    retf
_set_projection endp
; -----------------------------------------------------------------------------
; Purpose: Project a vector into screen-space coordinates.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_point2dout (word ptr
; 8); arg_vec (word ptr 6).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _projection_center_x, _projection_center_y, _projection_x_scale, _projection_y_scale.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _vector_to_point (original line 254)
; ASMDOC-GEOMETRY Purpose: Project signed 3D vector to signed 2D point with guarded unsigned division and
; saturation sentinels.
; ASMDOC-GEOMETRY Proposed public: _vector_to_point
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: near DS pointer to VECTOR {x:i16,y:i16,z:i16} (6 bytes)
; ASMDOC-GEOMETRY Input: BP+8 word: near DS pointer to two output i16 words (4 bytes)
; ASMDOC-GEOMETRY Input: Read/store order: capture z, read x, store output.x, then read y and store output.y;
; overlapping pointers observe this order
; ASMDOC-GEOMETRY Output: If z<=0: output=(0x8000,0x8000)
; ASMDOC-GEOMETRY Output: For z>0: x=center_x+sign(x)*floor(abs(x)*u16(x_scale)/z), y=center_y-
; sign(y)*floor(abs(y)*u16(y_scale)/z), all word operations
; ASMDOC-GEOMETRY Output: Predivision guard uses signed z <= signed((product.high<<1)+bit15(product.low));
; rejected x gets +32000 for x>=0 or -32000 for x<0; rejected y gets -32000 for y>=0 or +32000 for y<0
; ASMDOC-GEOMETRY Output: Signed ADD overflow saturates based on wrapped result sign: negative => +32000,
; nonnegative => -32000; AX is path-dependent, not a return value
; ASMDOC-GEOMETRY Preserves: BP
; ASMDOC-GEOMETRY Preserves: SI
; ASMDOC-GEOMETRY Preserves: DI
; ASMDOC-GEOMETRY Preserves: DS
; ASMDOC-GEOMETRY Preserves: SS
; ASMDOC-GEOMETRY Preserves: ES
; ASMDOC-GEOMETRY Clobbers: AX
; ASMDOC-GEOMETRY Clobbers: BX
; ASMDOC-GEOMETRY Clobbers: CX
; ASMDOC-GEOMETRY Clobbers: DX
; ASMDOC-GEOMETRY Clobbers: FLAGS
; ASMDOC-GEOMETRY Global read: _projection_center_x
; ASMDOC-GEOMETRY Global read: _projection_center_y
; ASMDOC-GEOMETRY Global read: _projection_x_scale
; ASMDOC-GEOMETRY Global read: _projection_y_scale
; ASMDOC-GEOMETRY Detail: NEG of -32768 retains 0x8000 and unsigned MUL consumes it as 32768. Do not replace
; with unrestricted host signed arithmetic.
; ASMDOC-GEOMETRY Dead: false. Reachability: src/render.c:2106-2107,2259,2359,3335;
; src/polygon.c:293,348,383,400 and others
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_projectp proc far
    projection_invalid_coordinate = byte ptr -32768
    arg_vec = word ptr 6
    arg_point2dout = word ptr 8

    push    bp
    mov     bp, sp
    push    di
    push    si
    mov     si, [bp+arg_vec]
    mov     di, [bp+arg_point2dout]
    mov     cx, [si+VECTOR.vz]
    or      cx, cx
    jle     short vector_point_branch_path_04
    mov     ax, [si+VECTOR.vx]
    or      ax, ax
    jl      short vector_point_branch_path_02
    mul     _projection_x_scale
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short vector_point_branch_path_01
    inc     bx
vector_point_branch_path_01:
    cmp     cx, bx
    jle     short vector_point_branch_path_06
    div     cx
    add     ax, _projection_center_x
    jo      short vector_point_branch_path_05
    mov     [di], ax
    jmp     short vector_point_join_01
    nop ; retained code pad at an even offset; EVEN would emit no byte
vector_point_branch_path_02:
    neg     ax
    mul     _projection_x_scale
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short vector_point_branch_path_03
    inc     bx
vector_point_branch_path_03:
    cmp     cx, bx
    jle     short vector_point_branch_path_07
    div     cx
    neg     ax
    add     ax, _projection_center_x
    jo      short vector_point_branch_path_05
    mov     [di], ax
    jmp     short vector_point_join_01
    nop ; retained code pad at an even offset; EVEN would emit no byte
vector_point_branch_path_04:
    mov     ax, 32768
    mov     [di], ax
    mov     [di+2], ax
    jmp     short vector_point_loop_head_01

    even ; word-align the following entry
vector_point_branch_path_05:
    or      ax, ax
    jge     short vector_point_branch_path_07
vector_point_branch_path_06:
    mov     ax, 7D00h
    mov     [di], ax
    jmp     short vector_point_join_01

    even ; word-align the following entry
vector_point_branch_path_07:
    mov     ax, 8300h
    mov     [di], ax
; ASMDOC-GEOMETRY The x output has already been stored before input y is loaded; preserve this order for
; aliased buffers.
vector_point_join_01:
    mov     ax, [si+VECTOR.vy]
    or      ax, ax
    jl      short vector_point_branch_path_09
    mul     _projection_y_scale
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short vector_point_branch_path_08
    inc     bx
vector_point_branch_path_08:
    cmp     cx, bx
    jle     short vector_point_branch_path_12
    div     cx
    neg     ax
    add     ax, _projection_center_y
    jo      short vector_point_branch_path_11
    mov     [di+2], ax
vector_point_loop_head_01:
    pop     si
    pop     di
    pop     bp
    retf
vector_point_branch_path_09:
    neg     ax
    mul     _projection_y_scale
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short vector_point_branch_path_10
    inc     bx
vector_point_branch_path_10:
    cmp     cx, bx
    jle     short vector_point_branch_path_13
    div     cx
    add     ax, _projection_center_y
    jo      short vector_point_branch_path_11
    mov     [di+2], ax
    jmp     short vector_point_loop_head_01
vector_point_branch_path_11:
    or      ax, ax
    jl      short vector_point_branch_path_13
vector_point_branch_path_12:
    mov     ax, -7D00h
    mov     [di+2], ax
    jmp     short vector_point_loop_head_01
vector_point_branch_path_13:
    mov     ax, 7D00h
    mov     [di+2], ax
    jmp     short vector_point_loop_head_01
_projectp endp
; -----------------------------------------------------------------------------
; Purpose: Release the selected sprite window.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, DX, SI, DI, BP, SP, CS, DS, ES, SS; balanced explicit saves/restores observed:
; DI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _window_release_order_error_msg.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
; ASMDOC-GEOMETRY PUBLIC _sprite_free_window (original line 385)
; ASMDOC-GEOMETRY Purpose: Release the most recently allocated window descriptor and its far bitmap
; allocation; enforce LIFO descriptor ownership.
; ASMDOC-GEOMETRY Proposed public: _sprite_free_window
; ASMDOC-GEOMETRY Convention: FAR cdecl; BP established by PUSH BP / MOV BP,SP; BP+6 is first argument, caller
; removes arguments; RETF
; ASMDOC-GEOMETRY Input: BP+6 word: far sprite descriptor offset; BP+8 word: descriptor segment
; ASMDOC-GEOMETRY Input: es:descriptor+0/+2: FAR bitmap/SHAPE2D header pointer; bitmap+2 word: original
; allocated height
; ASMDOC-GEOMETRY Input: Descriptor must be the top allocation: descriptor.offset + 2*(bitmap.height+15) must
; equal CS:[62C4h] (_next_wnd_def in allocator owner)
; ASMDOC-GEOMETRY Output: Success decrements CS:[62C4h] by descriptor extent and calls _mmgr_release(bitmap
; far pointer)
; ASMDOC-GEOMETRY Output: Out-of-order release calls _fatal_error("Window Released Out of Order\r\n"); no
; return/cleanup path if fatal_error returns
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
; ASMDOC-GEOMETRY Clobbers: FLAGS
; ASMDOC-GEOMETRY Global read: CS:[62C4h] window descriptor bump pointer (allocator _next_wnd_def)
; ASMDOC-GEOMETRY Global read: _window_release_order_error_msg
; ASMDOC-GEOMETRY Global read: caller descriptor and bitmap header height
; ASMDOC-GEOMETRY Global write: CS:[62C4h]
; ASMDOC-GEOMETRY Global write: resource-manager allocation state via _mmgr_release
; ASMDOC-GEOMETRY Service/call: _mmgr_release FAR after setting DS=SS
; ASMDOC-GEOMETRY Service/call: _fatal_error FAR on LIFO mismatch
; ASMDOC-GEOMETRY Detail: Saves/restores caller DS/SI/DI/BP on success; ES is not restored. ES memory reads
; are heap descriptor access, not direct video-memory access.
; ASMDOC-GEOMETRY Detail: The LIFO extent uses bitmap height, not current clipped bottom stored in the sprite.
; ASMDOC-GEOMETRY Dead: false. Reachability: Many direct calls: src/frontend.c:687,697,964 etc.;
; render.c:3237; race.c:1710-1712; uires.c:201; trackedt.c:922-926
; ASMDOC-GEOMETRY ---------------------------------------------------------------------------
_sprite_free_window proc far
    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     es, word ptr [bp+8]
    mov     di, [bp+6]
    mov     ds, word ptr es:[di+2] ;/* PLATFORM(video): accesses display memory through ES. */
    mov     si, es:[di] ;/* PLATFORM(video): accesses display memory through ES. */
    mov     ax, [si+2]
    add     ax, 0Fh
    shl     ax, 1
    mov     dx, ax
    add     dx, di
    mov     bx, cs:[62C4h]
    cmp     bx, dx
    jnz     short window_free_error_path_01
    sub     bx, ax
    mov     cs:[62C4h], bx
    push    ds
    push    si
    mov     ax, ss
    mov     ds, ax
    call    far ptr _purgememory
    add     sp, 4
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
; ASMDOC-GEOMETRY Fatal branch has no RETF or normal epilogue. If fatal_error unexpectedly returns, control
; falls out of this owner.
window_free_error_path_01:
    mov     ax, ss
    mov     ds, ax
    lea     ax, ds:[_window_release_order_error_msg]
    push    ax
    call    far ptr _abortmessage
_sprite_free_window endp
S012_TEXT ends
end
