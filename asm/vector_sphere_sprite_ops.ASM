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
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _vector_op_base_z.
; Globals written: none identified.
; -----------------------------------------------------------------------------
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
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12).
; Register ABI: touches AX, BX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
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
vector_set_base_z:
    push    bp
    mov     bp, sp
    mov     ax, [bp+6]
    mov     ds:_vector_op_base_z, ax
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender sphere.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: _prerender_max_line_width, _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
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
; Purpose: Implement the legacy internal helper nopsub 3320E. Its internal flow uses noop_entry2_branch_path_01.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12); arg_8
;   (word ptr 14); arg_A (word ptr 16).
; Register ABI: touches AX, SI, BP, SP, CS, DS; balanced explicit saves/restores observed: DS, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: _sprite1.
; -----------------------------------------------------------------------------
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
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12).
; Register ABI: touches AX, BP, SP, CS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _sprite1.
; -----------------------------------------------------------------------------
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
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, CX, DI, BP, SP, ES; balanced explicit saves/restores observed: DI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): selects a video-memory segment. */
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
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
