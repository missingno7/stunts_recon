; INPUTDOC Module keyboard_numlock_disable (owner load_26b05); comment-only static contract review.
; INPUTDOC Audited INPUTDOC contracts qualify the older retained generic comments; no code or declarations changed.
; INPUTDOC Routine names below follow rename-plan.json; definitions retain their original spelling.
; INPUTDOC dead=true means statically unreferenced; no proof against arbitrary external/numeric callers or execution.
; INPUTDOC Public data: none
; INPUTDOC Routine _kb_shift_checking2 -> _kb_shift_checking2: Clear BDA NumLock bit and peek the normalized pending
; INPUTDOC key.
.8086
INCLUDE include\platform_hw.inc
extrn _kb_checking:far
S018_TEXT segment byte public 'CODE'
assume cs:S018_TEXT
public _kb_shift_checking2
; -----------------------------------------------------------------------------
; Purpose: Implement the keyboard or joystick service identified by kb shift checking2.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs
; and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _kb_shift_checking2 (original _kb_shift_checking2).
; INPUTDOC Purpose: Clear BDA NumLock bit and peek the normalized pending key.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: AX=kb_checking result; ES=0040h.
; INPUTDOC Preserves: With installed INT16: BX,CX,DX,SI,DI,BP,DS,SS; SP balanced.
; INPUTDOC Clobbers: AX,ES,FLAGS; INT16 external contract unknown.
; INPUTDOC Globals read: BDA 0040:0017; transitive INT16 queue
; INPUTDOC Globals written: BDA 0040:0017 := old AND DFh
; INPUTDOC Services: kb_checking -> INT16 AH=01
; INPUTDOC Reachability: Live: src/obj_seg031.c:156.
_kb_shift_checking2 proc far
 mov ax,40h
 mov es,ax
; INPUTDOC DFh clears only the BDA NumLock toggle bit; companion wrapper sets it.
 and byte ptr es:[17h],0DFh
 call far ptr _kb_checking
 ret
_kb_shift_checking2 endp
S018_TEXT ends
end
