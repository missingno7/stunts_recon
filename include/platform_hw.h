/* MCGA/BIOS/DOS hardware and input protocol values used by the original game. */

/* INT 16h extended-keyboard AX words: the scan code is AH and AL is zero. */
#define BIOS_KEY_F1 0x3B00 /* BIOS INT 16h scan word for F1. */
#define BIOS_KEY_F2 0x3C00 /* BIOS INT 16h scan word for F2. */
#define BIOS_KEY_F3 0x3D00 /* BIOS INT 16h scan word for F3. */
#define BIOS_KEY_F4 0x3E00 /* BIOS INT 16h scan word for F4. */
#define BIOS_KEY_F5 0x3F00 /* BIOS INT 16h scan word for F5. */
#define BIOS_KEY_F6 0x4000 /* BIOS INT 16h scan word for F6. */
#define BIOS_KEY_F7 0x4100 /* BIOS INT 16h scan word for F7. */
#define BIOS_KEY_F8 0x4200 /* BIOS INT 16h scan word for F8. */
#define BIOS_KEY_F9 0x4300 /* BIOS INT 16h scan word for F9. */
#define BIOS_KEY_F10 0x4400 /* BIOS INT 16h scan word for F10. */

/* DOS mouse-driver interrupt and BIOS mouse-initialization selector. */
#define PLATFORM_DOS_MOUSE_INTERRUPT 0x33 /* DOS mouse-driver entry point, INT 33h. */
#define PLATFORM_BIOS_MOUSE_INTERRUPT 0x15 /* BIOS system-service entry point, INT 15h. */
#define PLATFORM_BIOS_MOUSE_INIT_SELECTOR 0xC201 /* INT 15h AX selector for the mouse initialization service. */

/* MCGA mode 13h indexed display and the associated 6-bit RGB DAC palette buffer. */
#define PLATFORM_SCREEN_WIDTH_PIXELS 320 /* MCGA 320-pixel screen width. */
#define PLATFORM_SCREEN_HEIGHT_PIXELS 200 /* MCGA 200-scanline screen height. */
#define PLATFORM_PALETTE_RGB_BYTES 768 /* 256 palette entries times three RGB component bytes. */
#define PLATFORM_PALETTE_COLOR_COUNT 256 /* Number of 8-bit indexed palette entries. */
#define PLATFORM_VGA_COLOR_WHITE 15 /* VGA/EGA palette index used for white. */

/* INT 33h mouse-driver function numbers in AX. */
#define MOUSE_FN_RESET 0 /* Reset/query the DOS mouse driver. */
#define MOUSE_FN_SHOW_CURSOR 1 /* Show the DOS mouse cursor. */
#define MOUSE_FN_HIDE_CURSOR 2 /* Hide the DOS mouse cursor. */
#define MOUSE_FN_GET_POSITION_AND_BUTTONS 3 /* Read cursor coordinates and button state. */
#define MOUSE_FN_SET_POSITION 4 /* Set cursor coordinates. */
#define MOUSE_FN_SET_HORIZONTAL_RANGE 7 /* Set the horizontal cursor range. */
#define MOUSE_FN_SET_VERTICAL_RANGE 8 /* Set the vertical cursor range. */
#define MOUSE_FN_SET_PIXEL_RATIO 15 /* Set the mouse mickey-to-pixel ratio. */

/* BIOS INT 16h extended-key scan words (scan code in AH, zero in AL). */
#define KEY_SCAN_UP 0x4800 /* BIOS extended-key scan word for Up Arrow. */
#define KEY_SCAN_DOWN 0x5000 /* BIOS extended-key scan word for Down Arrow. */
#define KEY_SCAN_RIGHT 0x4D00 /* BIOS extended-key scan word for Right Arrow. */
#define KEY_SCAN_LEFT 0x4B00 /* BIOS extended-key scan word for Left Arrow. */
#define KEY_SCAN_HOME 0x4700 /* BIOS extended-key scan word for Home. */
#define KEY_SCAN_END 0x4F00 /* BIOS extended-key scan word for End. */
#define KEY_SCAN_INSERT 0x5200 /* BIOS extended-key scan word for Insert. */
#define KEY_SCAN_DELETE 0x5300 /* BIOS extended-key scan word for Delete. */

/* ASCII values returned by BIOS keyboard services for ordinary keys. */
#define KEY_ASCII_BACKSPACE 8 /* ASCII Backspace control character. */
#define KEY_ASCII_TAB 9 /* ASCII Horizontal Tab control character. */
#define KEY_ASCII_ENTER 13 /* ASCII Carriage Return for Enter. */
#define KEY_ASCII_ESCAPE 27 /* ASCII Escape control character. */
#define KEY_ASCII_SPACE 0x20 /* ASCII space character. */
#define KEY_ASCII_Z 0x7A /* ASCII lowercase 'z'. */

/* Port, segment, and interrupt constants shared with platform_hw.inc. */
#define R5HW_DOS_TERMINATE_INT 0x20 /* DOS terminate-process interrupt. */
#define R5HW_DOS_INT 0x21 /* DOS service interrupt. */
#define R5HW_BIOS_VIDEO_INT 0x10 /* BIOS video service interrupt. */
#define R5HW_BIOS_KEYBOARD_INT 0x16 /* BIOS keyboard service interrupt. */
#define R5HW_PIC_MASTER_COMMAND_PORT 0x20 /* Master PIC command port. */
#define R5HW_PIC_MASTER_MASK_PORT 0x21 /* Master PIC interrupt-mask port. */
#define R5HW_KEYBOARD_DATA_PORT 0x60 /* Keyboard controller data port. */
#define R5HW_KEYBOARD_CONTROL_PORT 0x61 /* Keyboard control and speaker gate port. */
#define R5HW_PIT_CHANNEL0_PORT 0x40 /* PIT channel-zero data port. */
#define R5HW_PIT_CONTROL_PORT 0x43 /* PIT mode/control port. */
#define R5HW_JOYSTICK_GAME_PORT 0x201 /* Game-port joystick input address. */
#define R5HW_VIDEO_MONO_CRTC_INDEX_PORT 0x3B4 /* Monochrome CRTC index port. */
#define R5HW_VIDEO_MONO_MISC_PORT 0x3BF /* Monochrome display control port. */
#define R5HW_VIDEO_MONO_MODE_PORT 0x3B8 /* Monochrome display mode port. */
#define R5HW_VIDEO_COLOR_CRTC_INDEX_PORT 0x3D4 /* Color CRTC index port. */
#define R5HW_VIDEO_STATUS_PORT 0x3DA /* Color display status port. */
#define R5HW_VIDEO_ATTRIBUTE_INDEX_PORT 0x3C0 /* VGA attribute-controller index/data port. */
#define R5HW_VIDEO_DAC_INDEX_PORT 0x3C8 /* VGA DAC write-index port. */
#define R5HW_VIDEO_DAC_DATA_PORT 0x3C9 /* VGA DAC data port. */
#define R5HW_VIDEO_SEQ_INDEX_PORT 0x3C4 /* VGA sequencer index port. */
#define R5HW_VIDEO_SEQ_DATA_PORT 0x3C5 /* VGA sequencer data port. */
#define R5HW_VIDEO_GFX_INDEX_PORT 0x3CE /* VGA graphics-controller index port. */
#define R5HW_VIDEO_GFX_DATA_PORT 0x3CF /* VGA graphics-controller data port. */
#define R5HW_VIDEO_GRAPHICS_SEGMENT 0xA000 /* VGA graphics aperture segment. */
#define R5HW_VIDEO_MONO_TEXT_SEGMENT 0xB000 /* Monochrome text-memory segment. */
#define R5HW_VIDEO_TEXT_SEGMENT 0xB800 /* Color text-memory segment. */
