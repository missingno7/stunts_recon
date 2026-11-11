; ASMDOC Module review: Placeholder group name replaced by raw bitmap AND-mask family; no split/merge.
; ASMDOC Scope: code-derived contracts below supersede broad legacy summaries without deleting them. No instructions,
; ASMDOC declarations, old comments or line endings changed.
; ASMDOC Public routines: _nopsub_339FA, _putpixel_iconMask. Stack offsets are post-prologue BP offsets.
.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_339FA
public _putpixel_iconMask
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 339FA.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_339FA; proposed name _shape2d_and_bitmap_hotspot
; ASMDOC Purpose: Subtract hotspot words +04/+06 from explicit x/y then AND raw shape bitmap into sprite1.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] source SHAPE2D offset; [BP+08] source SHAPE2D segment; Shape +00 width word; bitmap/run stream begins
; ASMDOC +10h; caller supplies valid memory/rows; no clipping; [BP+0A] x word; [BP+0C] y word; Shape +02 signed height; width>=1
; ASMDOC and positive height assumed.
; ASMDOC Outputs: No semantic register result; sprite1 pixels ANDed with source; width=1 also zeroes adjacent byte.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF=0.
; ASMDOC Globals read: _sprite1 bitmap segment(+2), CS row-table pointer(+0A),pitch(+14h),row offsets; source
; ASMDOC width/height/pixels/hotspot.
; ASMDOC Globals written: sprite1 backing pixels; width=1 additionally clears next pixel of each row.
; ASMDOC Services: none.
; ASMDOC Dead: true. Reachability: No runtime symbol reference or address-taking found in tracked src/ or asm/.
; ASMDOC Shared tail: Both entries save DS/SI/DI/BP and jump to putpixel_iconmask_shared_exit (91); common epilogue at 123
; ASMDOC restores them. Unlabeled header-origin prologue 79 is not reached.
; ASMDOC Edge cases: Width=1 takes line 145 word AND after AX=0 then LODSB: AH=0, clearing following destination byte. Width=0
; ASMDOC enters word LOOP with CX=0. Height<=0 still executes one row. No header height-derived span bounds.
_nopsub_339FA proc far
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
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [bp+arg_4]
    sub     ax, [si+4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    sub     ax, [si+6]
    mov     [bp+var_4], ax
    jmp     short putpixel_iconmask_shared_exit
    ; align 2
    db 144
_nopsub_339FA endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by putpixel iconMask.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _putpixel_iconMask; proposed name _putpixel_iconMask
; ASMDOC Purpose: AND raw shape bitmap into sprite1 at explicit x/y.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] source SHAPE2D offset; [BP+08] source SHAPE2D segment; Shape +00 width word; bitmap/run stream begins
; ASMDOC +10h; caller supplies valid memory/rows; no clipping; [BP+0A] x word; [BP+0C] y word; Shape +02 signed height; width>=1
; ASMDOC and positive height assumed.
; ASMDOC Outputs: No semantic register result; sprite1 pixels ANDed with source; width=1 also zeroes adjacent byte.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return.
; ASMDOC Clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF=0.
; ASMDOC Globals read: _sprite1 bitmap segment(+2), CS row-table pointer(+0A),pitch(+14h),row offsets; source
; ASMDOC width/height/pixels.
; ASMDOC Globals written: sprite1 backing pixels; width=1 additionally clears next pixel of each row.
; ASMDOC Services: none.
; ASMDOC Dead: false. Reachability: Direct callers src/obj_seg009.c:403,963,1029.
; ASMDOC Shared tail: Both entries save DS/SI/DI/BP and jump to putpixel_iconmask_shared_exit (91); common epilogue at 123
; ASMDOC restores them. Unlabeled header-origin prologue 79 is not reached.
; ASMDOC Edge cases: Width=1 takes line 145 word AND after AX=0 then LODSB: AH=0, clearing following destination byte. Width=0
; ASMDOC enters word LOOP with CX=0. Height<=0 still executes one row. No header height-derived span bounds.
_putpixel_iconMask proc far
    var_6 = word ptr -6
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
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [bp+arg_4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    mov     [bp+var_4], ax
    jmp     short putpixel_iconmask_shared_exit
    ; align 2
    db 144
; ASMDOC Unexported shape-header-origin alternate prologue; both public entries jump over it into common core.
    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
; ASMDOC Shared unclipped raw bitmap AND core; saved DS/SI/DI are restored at common epilogue, including hotspot entry.
putpixel_iconmask_shared_exit:
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     es, word ptr word ptr cs:[_sprite1+2]
    cld
    mov     dx, [si+2]
    mov     bx, word ptr cs:[_sprite1+20]
    mov     ax, [si]
    sub     bx, ax
    add     si, 10h
    shr     ax, 1
    mov     [bp+var_6], ax
    jnb     short putpixel_iconmask_read_loop_3
    jz      short putpixel_iconmask_when_zero
    inc     bx
putpixel_iconmask_read_loop:
    mov     cx, [bp+var_6]
putpixel_iconmask_read_loop_2:
    lodsw
    and     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    putpixel_iconmask_read_loop_2
    lodsb
    and     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, bx
    dec     dx
    jg      short putpixel_iconmask_read_loop
putpixel_iconmask_draw_loop:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
putpixel_iconmask_read_loop_3:
    mov     cx, [bp+var_6]
putpixel_iconmask_read_loop_4:
    lodsw
    and     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    putpixel_iconmask_read_loop_4
    add     di, bx
    dec     dx
    jg      short putpixel_iconmask_read_loop_3
    jmp     short putpixel_iconmask_draw_loop
; ASMDOC Width-one special case: SHR made AX=0; LODSB sets only AL. The word AND at 145 clears adjacent destination byte via
; ASMDOC AH=0. Preserve this observable quirk.
putpixel_iconmask_when_zero:
    inc     bx
putpixel_iconmask_read_loop_5:
    lodsb
    and     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, bx
    dec     dx
    jg      short putpixel_iconmask_read_loop_5
    jmp     short putpixel_iconmask_draw_loop
    ; align 2
    db 0
_putpixel_iconMask endp
S012_TEXT ends
end
