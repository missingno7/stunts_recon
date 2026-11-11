; ASMDOC Module: math_resource_helpers. Mixed small helpers: paired 30-byte sprite DESCRIPTORS, archive directory
; ASMDOC access, Q14 trigonometry, unsigned projection arithmetic and interrupt-protected timer reads. Does not
; ASMDOC contain pixel blitting loops.
; ASMDOC Static documentation candidate only. Original comments retained verbatim; these contracts correct generic
; ASMDOC earlier annotations.
; ASMDOC No exhaustive dynamic call graph. dead=true means no retained source reference in this bounded audit, not
; ASMDOC permission to remove bytes.
.8086
name math_resource_helpers
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
public _sinetable
; ASMDOC Exported DATA: 257 quarter-wave Q14 samples; do not disassemble as executable instructions.
_sinetable dw 0
    dw 101
    dw 201
    dw 302
    dw 402
    dw 503
    dw 603
    dw 704
    dw 804
    dw 904
    dw 1005
    dw 1105
    dw 1205
    dw 1306
    dw 1406
    dw 1506
    dw 1606
    dw 1706
    dw 1806
    dw 1906
    dw 2006
    dw 2105
    dw 2205
    dw 2305
    dw 2404
    dw 2503
    dw 2603
    dw 2702
    dw 2801
    dw 2900
    dw 2999
    dw 3098
    dw 3196
    dw 3295
    dw 3393
    dw 3492
    dw 3590
    dw 3688
    dw 3786
    dw 3883
    dw 3981
    dw 4078
    dw 4176
    dw 4273
    dw 4370
    dw 4467
    dw 4563
    dw 4660
    dw 4756
    dw 4852
    dw 4948
    dw 5044
    dw 5139
    dw 5235
    dw 5330
    dw 5425
    dw 5520
    dw 5614
    dw 5708
    dw 5803
    dw 5897
    dw 5990
    dw 6084
    dw 6177
    dw 6270
    dw 6363
    dw 6455
    dw 6547
    dw 6639
    dw 6731
    dw 6823
    dw 6914
    dw 7005
    dw 7096
    dw 7186
    dw 7276
    dw 7366
    dw 7456
    dw 7545
    dw 7635
    dw 7723
    dw 7812
    dw 7900
    dw 7988
    dw 8076
    dw 8163
    dw 8250
    dw 8337
    dw 8423
    dw 8509
    dw 8595
    dw 8680
    dw 8765
    dw 8850
    dw 8935
    dw 9019
    dw 9102
    dw 9186
    dw 9269
    dw 9352
    dw 9434
    dw 9516
    dw 9598
    dw 9679
    dw 9760
    dw 9841
    dw 9921
    dw 10001
    dw 10080
    dw 10159
    dw 10238
    dw 10316
    dw 10394
    dw 10471
    dw 10549
    dw 10625
    dw 10702
    dw 10778
    dw 10853
    dw 10928
    dw 11003
    dw 11077
    dw 11151
    dw 11224
    dw 11297
    dw 11370
    dw 11442
    dw 11514
    dw 11585
    dw 11656
    dw 11727
    dw 11797
    dw 11866
    dw 11935
    dw 12004
    dw 12072
    dw 12140
    dw 12207
    dw 12274
    dw 12340
    dw 12406
    dw 12472
    dw 12537
    dw 12601
    dw 12665
    dw 12729
    dw 12792
    dw 12854
    dw 12916
    dw 12978
    dw 13039
    dw 13100
    dw 13160
    dw 13219
    dw 13279
    dw 13337
    dw 13395
    dw 13453
    dw 13510
    dw 13567
    dw 13623
    dw 13678
    dw 13733
    dw 13788
    dw 13842
    dw 13896
    dw 13949
    dw 14001
    dw 14053
    dw 14104
    dw 14155
    dw 14206
    dw 14256
    dw 14305
    dw 14354
    dw 14402
    dw 14449
    dw 14497
    dw 14543
    dw 14589
    dw 14635
    dw 14680
    dw 14724
    dw 14768
    dw 14811
    dw 14854
    dw 14896
    dw 14937
    dw 14978
    dw 15019
    dw 15059
    dw 15098
    dw 15137
    dw 15175
    dw 15213
    dw 15250
    dw 15286
    dw 15322
    dw 15357
    dw 15392
    dw 15426
    dw 15460
    dw 15493
    dw 15525
    dw 15557
    dw 15588
    dw 15619
    dw 15649
    dw 15679
    dw 15707
    dw 15736
    dw 15763
    dw 15791
    dw 15817
    dw 15843
    dw 15868
    dw 15893
    dw 15917
    dw 15941
    dw 15964
    dw 15986
    dw 16008
    dw 16029
    dw 16049
    dw 16069
    dw 16088
    dw 16107
    dw 16125
    dw 16143
    dw 16160
    dw 16176
    dw 16192
    dw 16207
    dw 16221
    dw 16235
    dw 16248
    dw 16261
    dw 16273
    dw 16284
    dw 16295
    dw 16305
    dw 16315
    dw 16324
    dw 16332
    dw 16340
    dw 16347
    dw 16353
    dw 16359
    dw 16364
    dw 16369
    dw 16373
    dw 16376
    dw 16379
    dw 16381
    dw 16383
    dw 16384
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
assume cs:S012_TEXT
extrn _sprite1:byte
public _sprite_copy_both_to_arg
; -----------------------------------------------------------------------------
; Purpose: Copy the two sprite buffers into the supplied destination.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, CX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed operands.
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
_sprite_copy_both_to_arg proc far
 push bp
 mov bp,sp
 push ds
 push di
 push si
 lea si,_sprite1
 mov di,word ptr [bp+6]
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
_sprite_copy_both_to_arg endp

public 'CODE'
assume cs:S012_TEXT
extrn _sprite1:byte
public _sprite_copy_arg_to_both
; -----------------------------------------------------------------------------
; Purpose: Copy the supplied sprite image into both sprite buffers.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, CX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed operands.
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
_sprite_copy_arg_to_both proc far
 push bp
 mov bp,sp
 push ds
 push di
 push si
 mov si,word ptr [bp+6]
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
_sprite_copy_arg_to_both endp

public 'CODE'
assume cs:S012_TEXT
public _file_get_res_shape_count
; -----------------------------------------------------------------------------
; Purpose: Read the shape count from a resource record.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, SI, BP, SP, ES; balanced explicit saves/restores observed: SI; path-specific inputs and outputs follow the listed operands.
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
 mov si,word ptr [bp+6]
 mov ax,word ptr es:[si+4] ;/* PLATFORM(video): accesses display memory through ES. */
 pop si
 pop bp
 ret
_file_get_res_shape_count endp

public 'CODE'
assume cs:S012_TEXT
public _file_get_shape2d
; -----------------------------------------------------------------------------
; Purpose: Resolve a shape index to the corresponding 2D shape data.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, SI, BP, SP, ES; balanced explicit saves/restores observed: SI; path-specific inputs and outputs follow the listed operands.
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
_file_get_shape2d proc far
 push bp
 mov bp,sp
 push si
 mov es,[bp+8]
 mov si,word ptr [bp+6]
 mov ax,word ptr es:[si+4] ;/* PLATFORM(video): accesses display memory through ES. */
 shl ax,1
 shl ax,1
 mov bx,word ptr [bp+10]
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
 add ax,word ptr es:[bx] ;/* PLATFORM(video): accesses display memory through ES. */
 adc dx,word ptr es:[bx+2] ;/* PLATFORM(video): accesses display memory through ES. */
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
_file_get_shape2d endp

public 'CODE'
assume cs:S012_TEXT
public _nopsub_326BA
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 326BA.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, DX, SI, BP, SP, ES; balanced explicit saves/restores observed: SI; path-specific inputs and outputs follow the listed operands.
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
    mov si, word ptr [bp+6]
    mov bx, word ptr [bp+0Ah]
    shl bx, 1
    shl bx, 1
    mov ax, word ptr es:[bx+si+6]
    mov dx, word ptr es:[bx+si+8]
    mov si, word ptr [bp+0Ch]
    mov word ptr [si], ax
    mov word ptr [si+2], dx
    pop si
    pop bp
    retf
_nopsub_326BA endp

assume ds:DGROUP
public 'CODE'
public _sinfast
public _cosfast
; -----------------------------------------------------------------------------
; Purpose: Return the fast fixed-point sine approximation.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, BP, SP, CS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
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
_sinfast proc far
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
    and     bx, 3
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
_sinfast endp
; -----------------------------------------------------------------------------
; Purpose: Return the fast fixed-point cosine approximation.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
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
_cosfast proc far
     cos_fast_s = byte ptr 0
     cos_fast_r = byte ptr 2
    cos_fast_arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, [bp+cos_fast_arg_0]
    add     ax, 100h
    jmp     short code_sin_fast_main

    db 0
_cosfast endp

public 'CODE'
assume cs:S012_TEXT
public _nopsub_32738
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 32738.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
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
_nopsub_32738 proc far
    push bp
    mov bp, sp
    mov dx, word ptr [bp+8]
    mov ax, word ptr [bp+6]
    div word ptr [bp+0Ah]
    pop bp
    retf
_nopsub_32738 endp

public 'CODE'
public _nopsub_32746
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 32746.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
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
; ASMDOC Evidence: asm/math_resource_helpers.ASM:561
_nopsub_32746 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, _projection_x_scale
    mul     [bp+arg_0]
    pop     bp
    retf
_nopsub_32746 endp

public 'CODE'
public _nopsub_32751
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 32751.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
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
; ASMDOC Evidence: asm/math_resource_helpers.ASM:583
_nopsub_32751 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, _projection_y_scale
    mul     [bp+arg_0]
    pop     bp
    retf
_nopsub_32751 endp

public 'CODE'
public _projectiondata9_times_ratio
; -----------------------------------------------------------------------------
; Purpose: Implement the graphics or geometry helper identified by projectiondata9 times ratio.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
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
_projectiondata9_times_ratio proc far
    push bp
    mov bp, sp
    mov ax, word ptr DGROUP:_projection_x_scale
    mul word ptr [bp+6]
    div word ptr [bp+8]
    pop bp
    retf
_projectiondata9_times_ratio endp

public 'CODE'
public _nopsub_3276A
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 3276A.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
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
_nopsub_3276A proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     ax, _projection_y_scale
    mul     [bp+arg_0]
    div     [bp+arg_2]
    pop     bp
    retf
_nopsub_3276A endp

public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _timer_get_counter
; -----------------------------------------------------------------------------
; Purpose: Read the 32-bit timer counter while interrupts are masked.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, DX, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
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
_timer_get_counter proc far
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov ax,word ptr ds:[_timer_callback_counter]
    mov dx,word ptr ds:[_timer_callback_counter+2]
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    ret
_timer_get_counter endp

public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _timer_custom_delta
; -----------------------------------------------------------------------------
; Purpose: Implement the timer service identified by timer custom delta.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
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
_timer_custom_delta proc far
    push bp
    mov bp, sp
    mov bx, word ptr [bp + 6]
    mov cx, word ptr [bp + 8]
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov ax, word ptr ds:[_timer_callback_counter]
    mov dx, word ptr ds:[_timer_callback_counter+2]
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    sub ax, bx
    sbb dx, cx
    pop bp
    retf
_timer_custom_delta endp
S012_TEXT ends
end
