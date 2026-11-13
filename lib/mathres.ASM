; ASMDOC Module: math_resource_helpers. Mixed small helpers: paired 30-byte sprite DESCRIPTORS, archive directory
; ASMDOC access, Q14 trigonometry, unsigned projection arithmetic and interrupt-protected timer reads. Does not
; ASMDOC contain pixel blitting loops.
; ASMDOC Static documentation candidate only. Original comments retained verbatim; these contracts correct generic
; ASMDOC earlier annotations.
; ASMDOC No exhaustive dynamic call graph. dead=true means no retained source reference in this bounded audit, not
; ASMDOC permission to remove bytes.
.8086
name math_resource_helpers
INCLUDE include\hw.inc
S012_TEXT segment word public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
public _sinetable
; ASMDOC Exported DATA: 257 quarter-wave Q14 samples; do not disassemble as executable instructions.
; Quarter-wave Q14: floor(16384*sin(i*pi/512)+0.5), i=0..256.
; Nearest integer, positive halves up; verified 257/257 entries, no exceptions.
_sinetable dw 0, 101, 201, 302, 402, 503, 603, 704
    dw 804, 904, 1005, 1105, 1205, 1306, 1406, 1506
    dw 1606, 1706, 1806, 1906, 2006, 2105, 2205, 2305
    dw 2404, 2503, 2603, 2702, 2801, 2900, 2999, 3098
    dw 3196, 3295, 3393, 3492, 3590, 3688, 3786, 3883
    dw 3981, 4078, 4176, 4273, 4370, 4467, 4563, 4660
    dw 4756, 4852, 4948, 5044, 5139, 5235, 5330, 5425
    dw 5520, 5614, 5708, 5803, 5897, 5990, 6084, 6177
    dw 6270, 6363, 6455, 6547, 6639, 6731, 6823, 6914
    dw 7005, 7096, 7186, 7276, 7366, 7456, 7545, 7635
    dw 7723, 7812, 7900, 7988, 8076, 8163, 8250, 8337
    dw 8423, 8509, 8595, 8680, 8765, 8850, 8935, 9019
    dw 9102, 9186, 9269, 9352, 9434, 9516, 9598, 9679
    dw 9760, 9841, 9921, 10001, 10080, 10159, 10238, 10316
    dw 10394, 10471, 10549, 10625, 10702, 10778, 10853, 10928
    dw 11003, 11077, 11151, 11224, 11297, 11370, 11442, 11514
    dw 11585, 11656, 11727, 11797, 11866, 11935, 12004, 12072
    dw 12140, 12207, 12274, 12340, 12406, 12472, 12537, 12601
    dw 12665, 12729, 12792, 12854, 12916, 12978, 13039, 13100
    dw 13160, 13219, 13279, 13337, 13395, 13453, 13510, 13567
    dw 13623, 13678, 13733, 13788, 13842, 13896, 13949, 14001
    dw 14053, 14104, 14155, 14206, 14256, 14305, 14354, 14402
    dw 14449, 14497, 14543, 14589, 14635, 14680, 14724, 14768
    dw 14811, 14854, 14896, 14937, 14978, 15019, 15059, 15098
    dw 15137, 15175, 15213, 15250, 15286, 15322, 15357, 15392
    dw 15426, 15460, 15493, 15525, 15557, 15588, 15619, 15649
    dw 15679, 15707, 15736, 15763, 15791, 15817, 15843, 15868
    dw 15893, 15917, 15941, 15964, 15986, 16008, 16029, 16049
    dw 16069, 16088, 16107, 16125, 16143, 16160, 16176, 16192
    dw 16207, 16221, 16235, 16248, 16261, 16273, 16284, 16295
    dw 16305, 16315, 16324, 16332, 16340, 16347, 16353, 16359
    dw 16364, 16369, 16373, 16376, 16379, 16381, 16383, 16384
    dw 16384
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
extrn _projection_x_scale:word
extrn _projection_y_scale:word
extrn _timer_callback_counter:dword
assume cs:S012_TEXT
S012_TEXT segment word public 'CODE'
extrn _sprite1:byte
public 'CODE'
extrn _sprite1:byte
public _savewindow
; -----------------------------------------------------------------------------
; Purpose: Copy the two sprite buffers into the supplied destination.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, CX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS,
; SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _sprite1.
; -----------------------------------------------------------------------------
; ASMDOC Contract _sprite_copy_both_to_arg (_sprite_copy_both_to_arg): Save the contiguous pair of 30-byte sprite
; ASMDOC descriptors with exactly 30 MOVSW operations.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 near DGROUP offset of 60-byte destination; DS=DGROUP on entry; CS:_sprite1 begins contiguous
; ASMDOC sprite pair
; ASMDOC outputs: 60 descriptor bytes copied CS:_sprite1 -> entry DS:argument; DF=0; ES=entry DS
; ASMDOC preserves: BX; DX; SI; DI; BP; DS; IF
; ASMDOC clobbers: AX; CX; ES; DF cleared
; ASMDOC globals_read: CS:_sprite1[60]
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: State snapshots include descriptor pointers and clipping/window fields; no framebuffer pixel data
; ASMDOC is copied. Forward word copy is not arbitrary-overlap memmove.
_savewindow proc far
 push bp
 mov bp,sp
 push ds
 push di
 push si
 lea si,_sprite1
 mov di,[bp+6]
 mov ax,ds
 mov es,ax
 mov ax,cs
 mov ds,ax
 cld
 mov cx,1Eh
 rep movsw
 pop si
 pop di
 pop ds
 pop bp
 ret
_savewindow endp

public 'CODE'
extrn _sprite1:byte
public _restorewindow
; -----------------------------------------------------------------------------
; Purpose: Copy the supplied sprite image into both sprite buffers.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, CX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS,
; SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _sprite1.
; -----------------------------------------------------------------------------
; ASMDOC Contract _sprite_copy_arg_to_both (_sprite_copy_arg_to_both): Restore the contiguous pair of 30-byte
; ASMDOC sprite descriptors with exactly 30 MOVSW operations.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 near DGROUP offset of 60-byte source; DS=DGROUP on entry; CS:_sprite1 begins contiguous
; ASMDOC sprite pair
; ASMDOC outputs: 60 descriptor bytes copied entry DS:argument -> CS:_sprite1; DF=0; ES=CS
; ASMDOC preserves: BX; DX; SI; DI; BP; DS; IF
; ASMDOC clobbers: AX; CX; ES; DF cleared
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: CS:_sprite1[60]
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: State snapshots include descriptor pointers and clipping/window fields; no framebuffer pixel data
; ASMDOC is copied. Forward word copy is not arbitrary-overlap memmove.
_restorewindow proc far
 push bp
 mov bp,sp
 push ds
 push di
 push si
 mov si,[bp+6]
 mov ax,cs
 mov es,ax
 lea di,_sprite1
 cld
 mov cx,1Eh
 rep movsw
 pop si
 pop di
 pop ds
 pop bp
 ret
_restorewindow endp

public 'CODE'
public _file_get_res_shape_count
; -----------------------------------------------------------------------------
; Purpose: Read the shape count from a resource record.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, SI, BP, SP, ES; balanced explicit saves/restores observed: SI; path-specific inputs
; and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _file_get_res_shape_count (_file_get_res_shape_count): Read archive count word at supplied far
; ASMDOC base+4.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 archive far offset; BP+8 archive far segment
; ASMDOC outputs: AX=u16 word from segment:[offset+4]; ES=archive segment
; ASMDOC preserves: BX; CX; DX; SI; DI; BP; DS; FLAGS
; ASMDOC clobbers: AX; ES
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: No format validation or bounds check. ES addresses arbitrary resource memory, not display memory.
_file_get_res_shape_count proc far
 push bp
 mov bp,sp
 push si
 mov es,[bp+8]
 mov si,[bp+6]
 mov ax,es:[si+4] ;/* PLATFORM(video): accesses display memory through ES. */
 pop si
 pop bp
 ret
_file_get_res_shape_count endp

public 'CODE'
public _shapepointer
; -----------------------------------------------------------------------------
; Purpose: Resolve a shape index to the corresponding 2D shape data.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, CX, DX, SI, BP, SP, ES; balanced explicit saves/restores observed: SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _file_get_shape2d (_file_get_shape2d): Compute normalized far shape pointer from count, indexed
; ASMDOC dword relative offset and archive segment.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 archive far offset (used only to read count at offset+4); BP+8 archive far segment; BP+10 u16
; ASMDOC shape index; no bounds check
; ASMDOC outputs: DX:AX=normalized segment:offset, offset 0..15; returned segment truncates so address retains low
; ASMDOC 20 linear bits; Linear computation starts at 16*archive_segment + ((8*count+6) mod 65536) + indexed offset
; ASMDOC dword; ES=archive segment
; ASMDOC preserves: SI; DI; BP; DS; DF; IF
; ASMDOC clobbers: AX; BX; CX; DX; ES; arithmetic flags
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Directory dword is read at ES:[(4*count+4*index+6) mod 65536], omitting supplied far offset SI.
; ASMDOC Payload base also omits SI. Therefore ordinary archive formula requires offset zero. Full additions use
; ASMDOC carry before normalization; no physical extent check.
_shapepointer proc far
 push bp
 mov bp,sp
 push si
 mov es,[bp+8]
 mov si,[bp+6]
 mov ax,es:[si+4] ;/* PLATFORM(video): accesses display memory through ES. */
 shl ax,1
 shl ax,1
 mov bx,[bp+10]
 shl bx,1
 shl bx,1
 add bx,ax
 add bx,6
 shl ax,1
 add ax,6
 mov cx,es
 xor dx,dx
 shl cx,1
 rcl dx,1
 shl cx,1
 rcl dx,1
 shl cx,1
 rcl dx,1
 shl cx,1
 rcl dx,1
 add ax,cx
 adc dx,0
 add ax,es:[bx] ;/* PLATFORM(video): accesses display memory through ES. */
 adc dx,es:[bx+2] ;/* PLATFORM(video): accesses display memory through ES. */
 mov bx,ax
 and bx,0Fh
 shr dx,1
 rcr ax,1
 shr dx,1
 rcr ax,1
 shr dx,1
 rcr ax,1
 shr dx,1
 rcr ax,1
 mov dx,ax
 mov ax,bx
 pop si
 pop bp
 ret
_shapepointer endp

public 'CODE'
public _nopsub_326BA
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 326BA.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, DX, SI, BP, SP, ES; balanced explicit saves/restores observed: SI; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_326BA (_mem_copy_table_dword): Copy an indexed four-byte entry from far base+6+4*index to
; ASMDOC near output; entry meaning unknown.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 far table offset; BP+8 far table segment; BP+10 u16 index (4*index wraps as word); BP+12 near
; ASMDOC DS output offset for four bytes
; ASMDOC outputs: DS:output receives low word then high word; DX:AX retains loaded entry dword; ES=table segment
; ASMDOC preserves: CX; SI; DI; BP; DS; DF; IF
; ASMDOC clobbers: AX; BX; DX; ES; arithmetic flags
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Notes: Unlike file_get_shape2d, this routine includes far base offset in the indexed reads. No evidence
; ASMDOC establishes SHAPE3D field roles. Both source words are loaded before either destination store.
_nopsub_326BA proc far
    push bp
    mov bp, sp
    push si
    mov es, word ptr [bp+8]
    mov si, [bp+6]
    mov bx, [bp+0Ah]
    shl bx, 1
    shl bx, 1
    mov ax, es:[bx+si+6]
    mov dx, es:[bx+si+8]
    mov si, [bp+0Ch]
    mov [si], ax
    mov [si+2], dx
    pop si
    pop bp
    retf
_nopsub_326BA endp

assume ds:DGROUP
public 'CODE'
public _intsin
public _intcos
; -----------------------------------------------------------------------------
; Purpose: Return the fast fixed-point sine approximation.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, BP, SP, CS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _sinetable.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _sinfast (_sinfast): Return Q14 sine using quadrant-reflected 257-word lookup.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 word angle, 1024 units per turn; only low 10 bits determine result; DS=DGROUP for sine table
; ASMDOC outputs: AX=signed Q14 value -16384..16384
; ASMDOC preserves: CX; DX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; BX; arithmetic flags
; ASMDOC globals_read: _sinetable[257]
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: JMP CS:sine_quadrant_dispatch[2*((angle>>8)&3)] uses four near code offsets embedded in code, not
; ASMDOC four instructions. Index reflected as 256-lowbyte for odd quadrants.
_intsin proc far
     sin_fast_s = byte ptr 0
     sin_fast_r = byte ptr 2
    sin_fast_arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, [bp+sin_fast_arg_0]
; ASMDOC Shared sine body entered by cosine with its own compatible BP frame; quadrant wraps every 1024 angle
; ASMDOC units.
code_sin_fast_main:
    mov     bl, ah
    xor     ah, ah
    and     bx, 03h
    shl     bx, 1
    jmp     cs:sine_quadrant_dispatch[bx]
; ASMDOC Inline CODE-segment dispatch DATA: four near offsets read by the preceding indirect JMP.
sine_quadrant_dispatch     dw offset sinfast_finish
    dw offset sinfast_finish_2
    dw offset sinfast_finish_3
    dw offset sinfast_finish_4
sinfast_finish:
    mov     bx, ax
    shl     bx, 1
    mov     ax, _sinetable[bx]
    pop     bp
    retf
sinfast_finish_2:
    mov     bx, 100h
    sub     bx, ax
    shl     bx, 1
    mov     ax, _sinetable[bx]
    pop     bp
    retf
sinfast_finish_3:
    mov     bx, ax
    shl     bx, 1
    mov     ax, _sinetable[bx]
    neg     ax
    pop     bp
    retf
sinfast_finish_4:
    mov     bx, 100h
    sub     bx, ax
    shl     bx, 1
    mov     ax, _sinetable[bx]
    neg     ax
    pop     bp
    retf
_intsin endp
; -----------------------------------------------------------------------------
; Purpose: Return the fast fixed-point cosine approximation.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; no named stack arguments referenced.
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _cosfast (_cosfast): Return Q14 cosine by adding one quarter-turn then joining sine body.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 word angle, 1024 units per turn; only low 10 bits determine result; DS=DGROUP for sine table
; ASMDOC outputs: AX=signed Q14 value -16384..16384
; ASMDOC preserves: CX; DX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; BX; arithmetic flags
; ASMDOC globals_read: _sinetable[257]
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Shares code_sin_fast_main and all sine epilogues; trailing DB 0 is alignment/unreached from entry.
_intcos proc far
     cos_fast_s = byte ptr 0
     cos_fast_r = byte ptr 2
    cos_fast_arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, [bp+cos_fast_arg_0]
    add     ax, 100h
    jmp     short code_sin_fast_main

    db 0 ; retained zero fill at this boundary
_intcos endp

public 'CODE'
public _thickness
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 32738.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_32738 (_math_div_u32_u16): Unsigned divide 32-bit stack numerator by 16-bit denominator.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 numerator low word; BP+8 numerator high word; BP+10 u16 denominator
; ASMDOC outputs: AX=u16 quotient; DX=u16 remainder if DIV succeeds
; ASMDOC preserves: BX; CX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; DX; DIV arithmetic flags undefined
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: none directly identified
; ASMDOC services: CPU INT0 on divisor zero or quotient overflow
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Notes: Success requires denominator!=0 and numerator/denominator<=FFFFh. Installed handler may alter fault
; ASMDOC recovery; do not interpret result after fault as normal division.
_thickness proc far
    push bp
    mov bp, sp
    mov dx, [bp+8]
    mov ax, [bp+6]
    div word ptr [bp+0Ah]
    pop bp
    retf
_thickness endp

public 'CODE'
public _scalethicknessxp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 32746.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _projection_x_scale.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_32746 (_projection_multiply_x_scale): Unsigned multiply projection_x_scale by supplied
; ASMDOC word, returning full product.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 u16 factor; DS=DGROUP
; ASMDOC outputs: DX:AX=u32 unsigned product
; ASMDOC preserves: BX; CX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; DX; MUL CF/OF; other arithmetic flags undefined
; ASMDOC globals_read: _projection_x_scale
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Evidence: lib/mathres.ASM:561
_scalethicknessxp proc far
    arg_factor = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, _projection_x_scale
    mul     [bp+arg_factor]
    pop     bp
    retf
_scalethicknessxp endp

public 'CODE'
public _scalethicknessyp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 32751.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _projection_y_scale.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_32751 (_projection_multiply_y_scale): Unsigned multiply projection_y_scale by supplied
; ASMDOC word, returning full product.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 u16 factor; DS=DGROUP
; ASMDOC outputs: DX:AX=u32 unsigned product
; ASMDOC preserves: BX; CX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; DX; MUL CF/OF; other arithmetic flags undefined
; ASMDOC globals_read: _projection_y_scale
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Evidence: lib/mathres.ASM:583
_scalethicknessyp proc far
    arg_factor = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, _projection_y_scale
    mul     [bp+arg_factor]
    pop     bp
    retf
_scalethicknessyp endp

public 'CODE'
public _thicknessxp
; -----------------------------------------------------------------------------
; Purpose: Implement the graphics or geometry helper identified by projectiondata9 times ratio.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _projection_x_scale.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _projectiondata9_times_ratio (_projectiondata9_times_ratio): Compute unsigned
; ASMDOC projection_x_scale*numerator/denominator with MUL then DIV.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 u16 numerator; BP+8 u16 denominator; DS=DGROUP
; ASMDOC outputs: AX=u16 quotient; DX=remainder if DIV succeeds
; ASMDOC preserves: BX; CX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; DX; DIV arithmetic flags undefined
; ASMDOC globals_read: _projection_x_scale
; ASMDOC globals_written: none directly identified
; ASMDOC services: CPU INT0 on divisor zero or quotient overflow
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: DX is remainder, not high product at return. No fixed-point shift, rounding increment or signed
; ASMDOC arithmetic is used.
_thicknessxp proc far
    push bp
    mov bp, sp
    mov ax, word ptr DGROUP:_projection_x_scale
    mul word ptr [bp+6]
    div word ptr [bp+8]
    pop bp
    retf
_thicknessxp endp

public 'CODE'
public _thicknessyp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 3276A.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _projection_y_scale.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_3276A (_projection_scale_y_ratio): Compute unsigned
; ASMDOC projection_y_scale*numerator/denominator with MUL then DIV.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 u16 numerator; BP+8 u16 denominator; DS=DGROUP
; ASMDOC outputs: AX=u16 quotient; DX=remainder if DIV succeeds
; ASMDOC preserves: BX; CX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; DX; DIV arithmetic flags undefined
; ASMDOC globals_read: _projection_y_scale
; ASMDOC globals_written: none directly identified
; ASMDOC services: CPU INT0 on divisor zero or quotient overflow
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Notes: DX is remainder, not high product at return. No fixed-point shift, rounding increment or signed
; ASMDOC arithmetic is used.
_thicknessyp proc far
    arg_numerator = word ptr 6
    arg_denominator = word ptr 8

    push    bp
    mov     bp, sp
    mov     ax, _projection_y_scale
    mul     [bp+arg_numerator]
    div     [bp+arg_denominator]
    pop     bp
    retf
_thicknessyp endp

public 'CODE'
public _gettick
; -----------------------------------------------------------------------------
; Purpose: Read the 32-bit timer counter while interrupts are masked.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, DX, DS; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _timer_callback_counter.
; Globals written: none identified.
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _timer_get_counter (_timer_get_counter): Read low/high words of callback counter with maskable
; ASMDOC interrupts disabled.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: DS=DGROUP; counter owned by timer ISR
; ASMDOC outputs: DX:AX=u32 logical callback counter snapshot; IF=1 after STI regardless of incoming IF
; ASMDOC preserves: BX; CX; SI; DI; BP; DS; ES; DF; arithmetic flags
; ASMDOC clobbers: AX; DX; IF forced enabled
; ASMDOC globals_read: _timer_callback_counter
; ASMDOC globals_written: none directly identified
; ASMDOC services: CLI; STI
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Does not preserve prior interrupt-enable state. Unit is callback ticks; no milliseconds conversion.
_gettick proc far
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov ax, word ptr ds:[_timer_callback_counter]
    mov dx, word ptr ds:[_timer_callback_counter+2]
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    ret
_gettick endp

public 'CODE'
public _tickcount
; -----------------------------------------------------------------------------
; Purpose: Implement the timer service identified by timer custom delta.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, CX, DX, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _timer_callback_counter.
; Globals written: none identified.
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _timer_custom_delta (_timer_custom_delta): Take interrupt-protected callback snapshot and
; ASMDOC subtract supplied dword modulo 2^32.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 previous counter low word; BP+8 previous counter high word; DS=DGROUP
; ASMDOC outputs: DX:AX=(snapshot-previous) modulo 2^32; BX:CX=previous low:high; IF=1
; ASMDOC preserves: SI; DI; BP; DS; ES; DF
; ASMDOC clobbers: AX; BX; CX; DX; arithmetic flags; IF forced enabled
; ASMDOC globals_read: _timer_callback_counter
; ASMDOC globals_written: none directly identified
; ASMDOC services: CLI; STI
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Notes: Unlike raw snapshot getter, subtraction changes arithmetic flags. No canonical reference found;
; ASMDOC dead export in current sources.
_tickcount proc far
    push bp
    mov bp, sp
    mov bx, [bp + 6]
    mov cx, [bp + 8]
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov ax, word ptr ds:[_timer_callback_counter]
    mov dx, word ptr ds:[_timer_callback_counter+2]
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    sub ax, bx
    sbb dx, cx
    pop bp
    retf
_tickcount endp
S012_TEXT ends
end
