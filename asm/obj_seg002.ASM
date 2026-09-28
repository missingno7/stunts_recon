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
    public _update_rpm_from_speed
    public _nopsub_19DE8
    public _nopsub_19DFF
    public _nopsub_19E09
    public _nopsub_19E13
    public _initialize_kevin_random
    public _get_kevinrandom_seed
    public _get_kevinrandom
    public _intr0_handler
    public _initialize_div0
    public _byte_19F07
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by update rpm from speed.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_currRpm (word ptr 6); arg_gearRatio (word ptr 10); arg_idleRpm (word ptr 14);
;   arg_isChangingGear (word ptr 12); arg_speed (word ptr 8).
; Register ABI: touches AX, CX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_update_rpm_from_speed proc far
     s = byte ptr 0
     r = byte ptr 2
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
skip_gear_adjustment:
    cmp     cx, [bp+arg_idleRpm]
    jnb     short rpm_ready_to_return
    mov     cx, [bp+arg_idleRpm]
rpm_ready_to_return:
    mov     ax, cx
    pop     bp
    pop     bp
    retf
_update_rpm_from_speed endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 19DE8. Its internal flow uses compare_value_prologue, clear_comparison_result.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_19DE8 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
compare_value_prologue:
    mov     bp, sp
    push    bp
clear_comparison_result:
    xor     ax, ax
load_comparison_argument:
    mov     bx, [bp+arg_0]
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
_nopsub_19DE8 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 19DFF. Its internal flow uses int61_set_frame, int61_load_argument.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(bios): calls a software interrupt service. */
; -----------------------------------------------------------------------------
_nopsub_19DFF proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
int61_set_frame:
    mov     bp, sp
int61_load_argument:
    mov     ax, [bp+arg_0]
int61_call_driver:
    int     61h ; reserved for user interrupt /* PLATFORM(bios): calls a software interrupt service. */
    pop     bp
int61_return:
    retf
_nopsub_19DFF endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 19E09. Its internal flow uses int60_set_frame, int60_load_argument.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(bios): calls a software interrupt service. */
; -----------------------------------------------------------------------------
_nopsub_19E09 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
int60_set_frame:
    mov     bp, sp
int60_load_argument:
    mov     ax, [bp+arg_0]
    int     60h ;/* PLATFORM(bios): calls a software interrupt service. */
    pop     bp
    retf
_nopsub_19E09 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 19E13. Its internal flow uses int62_set_frame, int62_save_index.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches SI, BP, SP; balanced explicit saves/restores observed: SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(bios): calls a software interrupt service. */
; -----------------------------------------------------------------------------
_nopsub_19E13 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
int62_set_frame:
    mov     bp, sp
int62_save_index:
    push    bp
    push    si
    mov     si, [bp+arg_0]
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
; Register ABI: touches BX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_initialize_kevin_random proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
seed_init_save_frame:
    push    bp
    mov     bx, [bp+arg_0]
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
_initialize_kevin_random endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by get kevinrandom seed.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches BX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_get_kevinrandom_seed proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    push    bp
    mov     bx, [bp+arg_0]
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
_get_kevinrandom_seed endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by get kevinrandom.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_get_kevinrandom proc far

    mov     al, _randomseeds+5
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
_get_kevinrandom endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by intr0 handler.
; ABI: FAR entry; source return form: IRET; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BP, SP, DS; balanced explicit saves/restores observed: DS; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _divide0_fault_cs, _divide0_fault_ip.
; /* PLATFORM(bios): returns from a hardware interrupt. */
; -----------------------------------------------------------------------------
_intr0_handler proc far
     s = byte ptr 0
     r = byte ptr 2

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
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches BX, DX, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _old_intr0_handler.
; Globals written: _old_intr0_handler.
; /* PLATFORM(dos): calls a DOS service. */
; -----------------------------------------------------------------------------
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
_byte_19F07     label byte
    push    ds
    lds     dx, _old_intr0_handler
    mov     ah, 25h
    mov     al, 0
    int     R5HW_DOS_INT ;/* PLATFORM(dos): calls a DOS service. */
_initialize_div0 endp
tail_19F13:
    pop ds
    retf
S002_TEXT ends
end
