; INPUTDOC Module keyboard_numlock_enable (owner load_26af4); comment-only static contract review.
; INPUTDOC Audited INPUTDOC contracts qualify the older retained generic comments; no code or declarations changed.
; INPUTDOC Routine names below follow rename-plan.json; definitions retain their original spelling.
; INPUTDOC dead=true means statically unreferenced; no proof against arbitrary external/numeric callers or execution.
; INPUTDOC Public data: none
; INPUTDOC Routine _keyboard_shift_checking1 -> _keyboard_shift_checking1: Set BDA NumLock bit and peek the normalized
; INPUTDOC pending key.
.8086
INCLUDE include\platform_hw.inc
extrn _nextkey:far
S018_TEXT segment word public 'CODE'
assume cs:S018_TEXT
public _keyboard_shift_checking1
; -----------------------------------------------------------------------------
; Purpose: Implement the keyboard or joystick service identified by keyboard shift checking1.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs
; and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; INPUTDOC Contract _keyboard_shift_checking1 (original _keyboard_shift_checking1).
; INPUTDOC Purpose: Set BDA NumLock bit and peek the normalized pending key.
; INPUTDOC Convention: 16-bit FAR caller-clean; entry SS:SP+0 return IP, +2 return CS; RETF (RET in FAR PROC is also
; INPUTDOC far).
; INPUTDOC Inputs: No stack arguments; DS=DGROUP when globals are used.
; INPUTDOC Outputs: AX=kb_checking result; ES=0040h.
; INPUTDOC Preserves: With installed INT16: BX,CX,DX,SI,DI,BP,DS,SS; SP balanced.
; INPUTDOC Clobbers: AX,ES,FLAGS; INT16 external contract unknown.
; INPUTDOC Globals read: BDA 0040:0017; transitive INT16 queue
; INPUTDOC Globals written: BDA 0040:0017 := old OR20h
; INPUTDOC Services: kb_checking -> INT16 AH=01
; INPUTDOC Reachability: Live: game/frontend.c:665.
_keyboard_shift_checking1 proc far
 mov ax,40h
 mov es,ax
; INPUTDOC 20h is the BDA NumLock toggle bit; this is a write followed by a non-consuming peek.
 or byte ptr es:[17h],20h
 call far ptr _nextkey
 ret
_keyboard_shift_checking1 endp
S018_TEXT ends
end
