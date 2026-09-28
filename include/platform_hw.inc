; Shared x86/DOS hardware constants used by the accepted MASM modules.
; Every R5HW_* symbol/value matches the same-named definition in
; include/platform_hw.h. This file contains MASM EQU data and comments only.
R5HW_DOS_TERMINATE_INT EQU 20h ; DOS terminate-process interrupt.
R5HW_DOS_INT EQU 21h ; DOS service interrupt.
R5HW_BIOS_VIDEO_INT EQU 10h ; BIOS video service interrupt.
R5HW_BIOS_KEYBOARD_INT EQU 16h ; BIOS keyboard service interrupt.
R5HW_PIC_MASTER_COMMAND_PORT EQU 20h ; Master PIC command port.
R5HW_PIC_MASTER_MASK_PORT EQU 21h ; Master PIC interrupt-mask port.
R5HW_KEYBOARD_DATA_PORT EQU 60h ; Keyboard controller data port.
R5HW_KEYBOARD_CONTROL_PORT EQU 61h ; Keyboard control and speaker gate port.
R5HW_PIT_CHANNEL0_PORT EQU 40h ; PIT channel-zero data port.
R5HW_PIT_CONTROL_PORT EQU 43h ; PIT mode/control port.
R5HW_JOYSTICK_GAME_PORT EQU 201h ; Game-port joystick input address.
R5HW_VIDEO_MONO_CRTC_INDEX_PORT EQU 3B4h ; Monochrome CRTC index port.
R5HW_VIDEO_MONO_MISC_PORT EQU 3BFh ; Monochrome display control port.
R5HW_VIDEO_MONO_MODE_PORT EQU 3B8h ; Monochrome display mode port.
R5HW_VIDEO_COLOR_CRTC_INDEX_PORT EQU 3D4h ; Color CRTC index port.
R5HW_VIDEO_STATUS_PORT EQU 3DAh ; Color display status port.
R5HW_VIDEO_ATTRIBUTE_INDEX_PORT EQU 3C0h ; VGA attribute-controller index/data port.
R5HW_VIDEO_DAC_INDEX_PORT EQU 3C8h ; VGA DAC write-index port.
R5HW_VIDEO_DAC_DATA_PORT EQU 3C9h ; VGA DAC data port.
R5HW_VIDEO_SEQ_INDEX_PORT EQU 3C4h ; VGA sequencer index port.
R5HW_VIDEO_SEQ_DATA_PORT EQU 3C5h ; VGA sequencer data port.
R5HW_VIDEO_GFX_INDEX_PORT EQU 3CEh ; VGA graphics-controller index port.
R5HW_VIDEO_GFX_DATA_PORT EQU 3CFh ; VGA graphics-controller data port.
R5HW_VIDEO_GRAPHICS_SEGMENT EQU 0A000h ; VGA graphics aperture segment.
R5HW_VIDEO_MONO_TEXT_SEGMENT EQU 0B000h ; Monochrome text-memory segment.
R5HW_VIDEO_TEXT_SEGMENT EQU 0B800h ; Color text-memory segment.
