; ASMDOC Module: rpm_rng_divide. RPM high-word multiply, six-byte RNG, signed-word sign helper, unknown INT60/61/62
; ASMDOC wrappers, and INT0 vector lifecycle. Mixed portable arithmetic and DOS boundary; no split proposed.
; ASMDOC Static documentation candidate only. Original comments retained verbatim; these contracts correct generic
; ASMDOC earlier annotations.
; ASMDOC No exhaustive dynamic call graph. dead=true means no retained source reference in this bounded audit, not
; ASMDOC permission to remove bytes.
INCLUDE include\platform_hw.inc
.model medium

_DATA segment word public 'DATA'
_old_intr0_handler dd 0
_divide0_fault_cs dw 0
_divide0_fault_ip dw 0
_DATA ends
dseg segment byte public 'STUNTSD'
    comm near _randomseeds:byte:6
dseg ends
DGROUP group _DATA, dseg

S002_TEXT segment byte public 'CODE'
    assume cs:S002_TEXT
    assume es:nothing, ss:nothing, ds:DGROUP
    public _calcrpm
    public _sgn
    public _nopsub_19DFF
    public _nopsub_19E09
    public _nopsub_19E13
    public _mysetseeds
    public _mygetseeds
    public _myrandom
    public _intr0_handler
    public _initialize_div0
    public _restore_div_zero_vector
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by update rpm from speed.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_currRpm (word ptr 6);
; arg_gearRatio (word ptr 10); arg_idleRpm (word ptr 14);
;   arg_isChangingGear (word ptr 12); arg_speed (word ptr 8).
; Register ABI: touches AX, CX, DX, BP, SP; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _update_rpm_from_speed (_update_rpm_from_speed): Return max(unsigned idle RPM, existing RPM
; ASMDOC during gear change or high word of unsigned speed*gear ratio otherwise).
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 u16 current RPM; BP+8 u16 speed; BP+10 u16 gear ratio; BP+12 word changing-gear flag (zero
; ASMDOC enables multiply); BP+14 u16 idle RPM
; ASMDOC outputs: AX=u16 clamped RPM; CX=same RPM; DX=high product only when changing-gear flag is zero; otherwise
; ASMDOC unchanged
; ASMDOC preserves: BX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; CX; DX on multiply path; arithmetic flags
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Evidence: game/rpmrand.ASM:36
_calcrpm proc far
    arg_currRpm = word ptr 6
    arg_speed = word ptr 8
    arg_gearRatio = word ptr 10
    arg_isChangingGear = word ptr 12
    arg_idleRpm = word ptr 14

    push    bp
    mov     bp, sp
    push    bp
    mov     cx, [bp+arg_currRpm]
    mov     ax, [bp+arg_speed]
    cmp     [bp+arg_isChangingGear], 0
    jnz     short skip_gear_adjustment
    mul     [bp+arg_gearRatio]
    mov     cx, dx
; ASMDOC Unsigned MUL uses only DX as candidate RPM; changing-gear path keeps current RPM in CX.
skip_gear_adjustment:
    cmp     cx, [bp+arg_idleRpm]
    jnb     short rpm_ready_to_return
    mov     cx, [bp+arg_idleRpm]
rpm_ready_to_return:
    mov     ax, cx
    pop     bp
    pop     bp
    retf
_calcrpm endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 19DE8. Its internal flow uses compare_value_prologue,
; clear_comparison_result.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_19DE8 (_math_sign_word): Return the sign of a signed 16-bit word (-1,0,+1).
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 i16 value
; ASMDOC outputs: AX=FFFFh,0,1 for negative,zero,positive; BX=input word
; ASMDOC preserves: CX; DX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; BX; arithmetic flags
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Evidence: game/rpmrand.ASM:71
_sgn proc far
    arg_value = word ptr 6

    push    bp
compare_value_prologue:
    mov     bp, sp
    push    bp
clear_comparison_result:
    xor     ax, ax
load_comparison_argument:
    mov     bx, [bp+arg_value]
test_comparison_argument:
    or      bx, bx
comparison_zero:
    jz      short return_nonpositive_result
comparison_negative:
    jl      short return_negative_result
comparison_positive:
    inc     ax
return_positive_result:
    pop     bp
    pop     bp
sign_word_positive_return:
    retf
return_negative_result:
    dec     ax
return_nonpositive_result:
    pop     bp
    pop     bp
sign_word_nonpositive_return:
    retf
_sgn endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 19DFF. Its internal flow uses int61_set_frame,
; int61_load_argument.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(bios): calls a software interrupt service. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_19DFF (_dos_call_int61_ax): Pass an opaque word through AX to software INT 61h; provider
; ASMDOC and operation unknown.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 word opaque service argument; Other registers/segments may be service inputs; unknown
; ASMDOC outputs: Service-defined results; no result conversion
; ASMDOC preserves: BP if interrupt respects frame/stack
; ASMDOC clobbers: AX loaded before interrupt; Other effects unknown: interrupt provider not identified
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: none directly identified
; ASMDOC services: INT 61h (user interrupt; not established BIOS or audio API)
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Notes: Do not infer preserved registers, globals, memory effects, or device semantics from the wrapper
; ASMDOC alone.
_nopsub_19DFF proc far
    arg_service_argument = word ptr 6

    push    bp
int61_set_frame:
    mov     bp, sp
int61_load_argument:
    mov     ax, [bp+arg_service_argument]
int61_call_driver:
    int     61h ; reserved for user interrupt /* PLATFORM(bios): calls a software interrupt service. */
    pop     bp
int61_return:
    retf
_nopsub_19DFF endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 19E09. Its internal flow uses int60_set_frame,
; int60_load_argument.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(bios): calls a software interrupt service. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_19E09 (_dos_call_int60_ax): Pass an opaque word through AX to software INT 60h; provider
; ASMDOC and operation unknown.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 word opaque service argument; Other registers/segments may be service inputs; unknown
; ASMDOC outputs: Service-defined results; no result conversion
; ASMDOC preserves: BP if interrupt respects frame/stack
; ASMDOC clobbers: AX loaded before interrupt; Other effects unknown: interrupt provider not identified
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: none directly identified
; ASMDOC services: INT 60h (user interrupt; not established BIOS or audio API)
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Notes: Do not infer preserved registers, globals, memory effects, or device semantics from the wrapper
; ASMDOC alone.
_nopsub_19E09 proc far
    arg_service_argument = word ptr 6

    push    bp
int60_set_frame:
    mov     bp, sp
int60_load_argument:
    mov     ax, [bp+arg_service_argument]
    int     60h ;/* PLATFORM(bios): calls a software interrupt service. */
    pop     bp
    retf
_nopsub_19E09 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 19E13. Its internal flow uses int62_set_frame,
; int62_save_index.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches SI, BP, SP; balanced explicit saves/restores observed: SI; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(bios): calls a software interrupt service. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _nopsub_19E13 (_dos_call_int62_si): Pass an opaque word through SI to software INT 62h; provider
; ASMDOC and operation unknown.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 word opaque service argument; Other registers/segments may be service inputs; unknown
; ASMDOC outputs: Service-defined results; no result conversion
; ASMDOC preserves: BP if interrupt respects frame/stack; SI explicitly saved and restored
; ASMDOC clobbers: SI loaded before interrupt; Other effects unknown: interrupt provider not identified
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: none directly identified
; ASMDOC services: INT 62h (user interrupt; not established BIOS or audio API)
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Notes: Do not infer preserved registers, globals, memory effects, or device semantics from the wrapper
; ASMDOC alone.
_nopsub_19E13 proc far
    arg_service_argument = word ptr 6

    push    bp
int62_set_frame:
    mov     bp, sp
int62_save_index:
    push    bp
    push    si
    mov     si, [bp+arg_service_argument]
int62_call_driver:
    int     62h ; reserved for user interrupt /* PLATFORM(bios): calls a software interrupt service. */
    pop     si
int62_restore_index_and_return:
    pop     bp
    pop     bp
    retf
_nopsub_19E13 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by initialize kevin random.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches BX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _initialize_kevin_random (_initialize_kevin_random): Copy six consecutive bytes from a near
; ASMDOC source into the RNG state in ascending address order.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 near DS offset of six source bytes; DS=DGROUP
; ASMDOC outputs: randomseeds[0..5] replaced; AL=last source byte; AH unchanged; BX=source offset
; ASMDOC preserves: CX; DX; SI; DI; BP; DS; ES; FLAGS
; ASMDOC clobbers: AL; BX
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: _randomseeds[6]
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Six individual byte loads/stores; overlapping source/state aliases observe sequential copying, not
; ASMDOC an abstract snapshot.
_mysetseeds proc far
    arg_seed_source = word ptr 6

    push    bp
    mov     bp, sp
seed_init_save_frame:
    push    bp
    mov     bx, [bp+arg_seed_source]
    mov     al, [bx]
seed_init_store_byte0:
    mov     _randomseeds, al
    mov     al, [bx+1]
seed_init_store_byte1:
    mov     _randomseeds+1, al
seed_init_store_byte2:
    mov     al, [bx+2]
seed_init_store_byte3:
    mov     _randomseeds+2, al
    mov     al, [bx+3]
    mov     _randomseeds+3, al
    mov     al, [bx+4]
    mov     _randomseeds+4, al
    mov     al, [bx+5]
    mov     _randomseeds+5, al
    pop     bp
seed_init_return:
    pop     bp
    retf
_mysetseeds endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by get kevinrandom seed.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches BX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _get_kevinrandom_seed (_get_kevinrandom_seed): Copy the six RNG bytes into a near destination in
; ASMDOC ascending address order.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: BP+6 near DS offset of six writable destination bytes; DS=DGROUP
; ASMDOC outputs: DS:destination[0..5] written; AL=last state byte; AH unchanged; BX=destination offset
; ASMDOC preserves: CX; DX; SI; DI; BP; DS; ES; FLAGS
; ASMDOC clobbers: AL; BX
; ASMDOC globals_read: _randomseeds[6]
; ASMDOC globals_written: none directly identified
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: May overwrite RNG state if destination aliases it; individual byte operations retain alias
; ASMDOC ordering.
_mygetseeds proc far
    arg_seed_destination = word ptr 6

    push    bp
    mov     bp, sp
    push    bp
    mov     bx, [bp+arg_seed_destination]
    mov     al, _randomseeds
    mov     [bx], al
    mov     al, _randomseeds+1
    mov     [bx+1], al
    mov     al, _randomseeds+2
    mov     [bx+2], al
    mov     al, _randomseeds+3
    mov     [bx+3], al
    mov     al, _randomseeds+4
    mov     [bx+4], al
seed_copy_byte5:
    mov     al, _randomseeds+5
    mov     [bx+5], al
    pop     bp
    pop     bp
    retf
_mygetseeds endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by get kevinrandom.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _get_kevinrandom (_get_kevinrandom): Update six-byte RNG by descending cumulative byte sums
; ASMDOC followed by carry-propagating increment from byte 5 to byte 0.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: DS=DGROUP
; ASMDOC outputs: AX=zero-extended updated randomseeds[0] (0..255)
; ASMDOC preserves: BX; CX; DX; SI; DI; BP; DS; ES; DF; IF
; ASMDOC clobbers: AX; arithmetic flags
; ASMDOC globals_read: _randomseeds[6]
; ASMDOC globals_written: _randomseeds[6]
; ASMDOC services: none directly identified
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Every ADD wraps modulo 256 without ADC. Increment carry is implemented by testing byte wrap, after
; ASMDOC cumulative additions.
_myrandom proc far

    mov     al, _randomseeds+5
; ASMDOC Cumulative byte ADDs intentionally discard carry; the later INC chain propagates only increment wrap.
random_add_seed_byte4:
    add     al, _randomseeds+4
random_store_seed_byte4:
    mov     _randomseeds+4, al
random_add_seed_byte3:
    add     al, _randomseeds+3
random_store_seed_byte3:
    mov     _randomseeds+3, al
random_add_seed_byte2:
    add     al, _randomseeds+2
random_store_seed_byte2:
    mov     _randomseeds+2, al
    add     al, _randomseeds+1
random_store_seed_byte1:
    mov     _randomseeds+1, al
    add     al, _randomseeds
    mov     _randomseeds, al
    inc     _randomseeds+5
    jnz     short random_return_seed_byte0
    inc     _randomseeds+4
    jnz     short random_return_seed_byte0
    inc     _randomseeds+3
    jnz     short random_return_seed_byte0
    inc     _randomseeds+2
    jnz     short random_return_seed_byte0
    inc     _randomseeds+1
    jnz     short random_return_seed_byte0
    inc     _randomseeds
random_return_seed_byte0:
    mov     al, _randomseeds
    xor     ah, ah
    retf
_myrandom endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by intr0 handler.
; ABI: FAR entry; source return form: IRET; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BP, SP, DS; balanced explicit saves/restores observed: DS; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _divide0_fault_cs, _divide0_fault_ip.
; /* PLATFORM(bios): returns from a hardware interrupt. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _intr0_handler (_intr0_handler): Record fault CS:IP, advance saved IP by exactly two bytes, zero
; ASMDOC AX, and resume with IRET.
; ASMDOC Convention: CPU interrupt entry, not callable C FAR ABI; IRET consumes IP,CS,FLAGS.
; ASMDOC inputs: CPU interrupt frame at entry SP: IP,+2 CS,+4 FLAGS; After PUSH BP: BP+2 saved IP, BP+4 saved CS,
; ASMDOC BP+6 saved FLAGS
; ASMDOC outputs: _divide0_fault_cs=interrupted CS; _divide0_fault_ip=interrupted IP; Saved IP +=2 modulo 65536;
; ASMDOC AX=0; IRET restores interrupted FLAGS and modified CS:IP
; ASMDOC preserves: BX; CX; DX; SI; DI; BP; DS; ES; interrupted FLAGS via IRET
; ASMDOC clobbers: AX; saved interrupt IP
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: _divide0_fault_cs; _divide0_fault_ip
; ASMDOC services: INT0 exception entry / IRET
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Installed indirectly by initialize_div0. Does not save interrupted AX and does not chain old
; ASMDOC vector. Two-byte recovery is not general instruction decoding.
_intr0_handler proc far
    push    bp
    mov     bp, sp
    push    ds
    mov     ax, seg dseg
div0_load_data_segment:
    mov     ds, ax
div0_save_fault_cs:
    mov     ax, [bp+4]
div0_save_fault_ip:
    mov     _divide0_fault_cs, ax
div0_prepare_resume_ip:
    mov     ax, [bp+2]
    mov     _divide0_fault_ip, ax
; ASMDOC Skip exactly two bytes in saved interrupt IP. AX is deliberately destroyed and becomes zero.
div0_advance_resume_ip:
    inc     ax
    inc     ax
div0_store_resume_ip:
    mov     [bp+2], ax
div0_clear_return_value:
    xor     ax, ax
    pop     ds
    pop     bp
    iret ;/* PLATFORM(bios): returns from a hardware interrupt. */
_intr0_handler endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by initialize div0.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches BX, DX, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _old_intr0_handler.
; Globals written: _old_intr0_handler.
; /* PLATFORM(dos): calls a DOS service. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _initialize_div0 (_initialize_div0): Save old INT0 vector then install this module's divide
; ASMDOC handler.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: DS=DGROUP for storing old vector
; ASMDOC outputs: _old_intr0_handler=ES:BX returned by DOS AH=35h; INT0 vector becomes CS:_intr0_handler
; ASMDOC preserves: DS explicitly saved/restored; Other register preservation depends on DOS service contract
; ASMDOC clobbers: AX; BX; DX; ES; flags/service-defined effects
; ASMDOC globals_read: none directly identified
; ASMDOC globals_written: _old_intr0_handler
; ASMDOC services: INT21h AH=35h AL=0 get vector; INT21h AH=25h AL=0 set vector
; ASMDOC Reachability: live; direct or indirect reference retained.
; ASMDOC Notes: Normal RETF precedes restore label: install does not fall through to restoration.
_initialize_div0 proc far

    push    ds
div0_get_old_vector:
    mov     ah, 35h ; '5'
div0_select_int0:
    mov     al, 0
    int     R5HW_DOS_INT ; DOS - 2+ - GET INTERRUPT VECTOR /* PLATFORM(dos): calls a DOS service. */
div0_capture_old_segment:
    mov     word ptr _old_intr0_handler+2, es
div0_capture_old_offset:
    mov     word ptr _old_intr0_handler, bx
div0_select_game_segment:
    mov     dx, seg S002_TEXT
div0_load_game_segment:
    mov     ds, dx
div0_set_handler_offset:
    mov     dx, offset _intr0_handler
div0_select_set_vector_function:
    mov     ah, 25h ; '%'
div0_select_int0_vector:
    mov     al, 0
    int     R5HW_DOS_INT ; DOS - SET INTERRUPT VECTOR /* PLATFORM(dos): calls a DOS service. */
    pop     ds
div0_return:
    retf
; ASMDOC Contract _restore_div_zero_vector (_restore_div_zero_vector): Restore DOS INT0 vector from saved far
; ASMDOC pointer.
; ASMDOC Convention: FAR, caller removes arguments; BP offsets below are after PUSH BP / MOV BP,SP (first argument
; ASMDOC BP+6 = entry SP+4); RETF, including MASM RET inside PROC FAR.
; ASMDOC inputs: DS=DGROUP; initialize_div0 must have initialized saved vector
; ASMDOC outputs: INT0 vector restored; DS restored
; ASMDOC preserves: DS explicitly saved/restored; BP untouched by wrapper; Other preservation depends on DOS
; ASMDOC service contract
; ASMDOC clobbers: AX; DX; flags/service-defined effects
; ASMDOC globals_read: _old_intr0_handler
; ASMDOC globals_written: none directly identified
; ASMDOC services: INT21h AH=25h AL=0 set vector
; ASMDOC Reachability: dead in current static source/reference plan.
; ASMDOC Notes: Public LABEL BYTE inside initialize_div0 PROC, independently callable FAR entry. Epilogue POP DS /
; ASMDOC RETF lies after ENDP at tail_19F13. No canonical reference found; retained dead export.
_restore_div_zero_vector     label byte
    push    ds
    lds     dx, _old_intr0_handler
    mov     ah, 25h
    mov     al, 0
    int     R5HW_DOS_INT ;/* PLATFORM(dos): calls a DOS service. */
_initialize_div0 endp
; ASMDOC Shared restore-vector epilogue lies outside PROC extent but belongs to the public restore entry.
tail_19F13:
    pop ds
    retf
S002_TEXT ends
end
