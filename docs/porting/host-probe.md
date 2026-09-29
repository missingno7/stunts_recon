# Host compiler probe: porting hazard proposal

The 38-source report below is the original `--mode legacy` shim survey. The declaration audit has two port-only modes. After the port-only declaration/adapter fix, both `python tools/porting/host_probe.py --mode compat` and `--mode strict-central` compile 38/38 TUs for syntax and object output. The integ55 freeze-gate receipt preserves the earlier strict-central count of 36/38. See [port-only headers and TU audit](port-headers.md) for the interface changes and remaining runtime residuals. These probes do not link or execute the game.

Generated from the active accepted C source set by `tools/porting/host_probe.py --mode legacy`.
The probe compiles each active `GAME_C` C recipe source with GCC for the current MinGW host, once with `-fsyntax-only` and once with `-c`, using warnings and the host compatibility shims. It does not link or run the game.

Compiler: `C:\msys64\mingw64\bin\gcc.exe` (gcc.exe (Rev10, Built by MSYS2 project) 12.2.0); target `x86_64-w64-mingw32`
Accepted C sources: 38

## Aggregate

- Syntax-only failures: 8/38; object compile failures: 8/38.
- Distinct GCC warning diagnostics: 1840; distinct error diagnostics: 88 (same line/message across the two passes counted once).
- `I16/U16/I32/U32` are mapped to fixed-width types. Bare `int` remains host 32-bit while original MSC 5.x `int` is 16-bit; flat pointers are host-sized because segmented qualifiers are erased.
- Lexical markers are review requests; compiler diagnostics are reported separately. Errors from missing legacy declarations or incompatible declarations are actionable host-port blockers.

## Category coverage

- **implicit int widths:** source markers in 11 files; compiler diagnostics in 22 files (1624 distinct diagnostic rows).
- **pointer size assumptions:** source markers in 0 files; compiler diagnostics in 8 files (51 distinct diagnostic rows).
- **segment arithmetic:** source markers in 2 files; compiler diagnostics in 0 files (0 distinct diagnostic rows).
- **inline asm:** source markers in 1 files; compiler diagnostics in 1 files (1 distinct diagnostic rows).
- **int86/port I/O:** source markers in 1 files; compiler diagnostics in 0 files (0 distinct diagnostic rows).
- **far pointer normalization:** source markers in 14 files; compiler diagnostics in 2 files (13 distinct diagnostic rows).
- **signed shift:** source markers in 18 files; compiler diagnostics in 0 files (0 distinct diagnostic rows).
- **struct packing:** source markers in 27 files; compiler diagnostics in 1 files (1 distinct diagnostic rows).
- **other compiler diagnostic:** source markers in 0 files; compiler diagnostics in 35 files (250 distinct diagnostic rows).

## Per-file hazards

### `src/audio_make_filename.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 3 warnings, 0 errors.
- Existing tags: none
- **implicit int widths:** compiler diagnostic; 2 distinct warning/error lines
  - `src\audio_make_filename.c:4:12: warning: conflicting types for built-in function 'strrchr'; expected 'char *(const char *, int)' [-Wbuiltin-declaration-mismatch]`
  - `src\audio_make_filename.c:7:12: warning: conflicting types for built-in function 'strlen'; expected 'long long unsigned int(const char *)' [-Wbuiltin-declaration-mismatch]`
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\audio_make_filename.c:11:5: warning: no previous prototype for 'audio_make_filename' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for implicit int widths

### `src/fardata_11036.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- Existing tags: none
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit)
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer; 1 distinct warning/error lines
  - `src\fardata_11036.c:11:8: warning: missing initializer for field 'plane_rotation' of 'struct PLANE' [-Wmissing-field-initializers]`
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- Tagging follow-up: add/review `PORT:` coverage for implicit int widths; add/review `PORT:` coverage for far pointer normalization; add/review `PORT:` coverage for struct packing

### `src/fardata_11039.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 0 warnings, 0 errors.
- Existing tags: none
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- Tagging follow-up: add/review `PORT:` coverage for far pointer normalization

### `src/file_get_unflip_size.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 2 warnings, 0 errors.
- Existing tags: PORT: Resource headers use MSC 16-bit int layout; assert these field offsets in a host port.@6
- **implicit int widths:** compiler diagnostic; 1 distinct warning/error lines
  - `src\file_get_unflip_size.c:32:16: warning: conversion from 'int' to 'uint16_t' {aka 'short unsigned int'} may change value [-Wconversion]`
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\file_get_unflip_size.c:22:9: warning: no previous prototype for 'file_get_unflip_size' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for signed shift

### `src/file_load_shape2d_expandedsize.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 6 warnings, 0 errors.
- Existing tags: PORT: Resource headers use MSC 16-bit int layout; assert these field offsets in a host port.@5; PORT: The 16-bit compiler accumulates this size as long before narrowing to int.@28
- **implicit int widths:** compiler diagnostic; 5 distinct warning/error lines
  - `src\file_load_shape2d_expandedsize.c:20:27: warning: conversion to 'long long unsigned int' from 'int' may change the sign of the result [-Wsign-conversion]`
  - `src\file_load_shape2d_expandedsize.c:23:18: warning: conversion from 'int' to 'uint16_t' {aka 'short unsigned int'} may change value [-Wconversion]`
  - `src\file_load_shape2d_expandedsize.c:25:14: warning: conversion to 'long long unsigned int' from 'int32_t' {aka 'int'} may change the sign of the result [-Wsign-conversion]`
  - ? 2 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\file_load_shape2d_expandedsize.c:14:9: warning: no previous prototype for 'file_load_shape2d_expandedsize' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for signed shift

### `src/heapsort_by_order.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- Existing tags: none
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\heapsort_by_order.c:5:6: warning: no previous prototype for 'heapsortorder' [-Wmissing-prototypes]`
- Tagging follow-up: the screened hazards have matching source PORT tags

### `src/nopsub_36AF2.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- Existing tags: none
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\nopsub_36AF2.c:5:10: warning: no previous prototype for 'nopsub_36AF2' [-Wmissing-prototypes]`
- Tagging follow-up: the screened hazards have matching source PORT tags

### `src/obj_seg000.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 150 warnings, 2 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- Existing tags: PLATFORM(audio)@449,561,611,621,658 (+7); PLATFORM(file)@450,478,479,480,551 (+43); PLATFORM(input_kb)@451,568,573,580,660 (+19); PLATFORM(memory)@452,484,612,652,672 (+11); PLATFORM(video)@453,481,662,673,681 (+229); PLATFORM(timer)@743,817,821,852,1237 (+15); PLATFORM(input_mouse)@885,922,926,980,1058 (+12); PORT: Layout uses MSC /Zp and target scalar widths.@18,30,31,32 (+12); PORT: Q8 value is converted at the legacy boundary.@1576,2288,2297,2304
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 139 distinct warning/error lines
  - `src\obj_seg000.c:140:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `src\obj_seg000.c:156:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `src\obj_seg000.c:163:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - ? 136 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 4 distinct warning/error lines
  - `src\obj_seg000.c:511:10: warning: pointer targets in assignment from 'uint8_t *' {aka 'unsigned char *'} to 'char *' differ in signedness [-Wpointer-sign]`
  - `src\obj_seg000.c:552:87: warning: pointer targets in passing argument 2 of 'file_read_fatal' differ in signedness [-Wpointer-sign]`
  - `src\obj_seg000.c:761:92: warning: passing argument 3 of 'locate_many_resources' from incompatible pointer type [-Wincompatible-pointer-types]`
  - ? 1 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 9 distinct warning/error lines
  - `src\obj_seg000.c:456:5: warning: first argument of 'main' should be 'int' [-Wmain]`
  - `src\obj_seg000.c:468:22: warning: suggest parentheses around '-' inside '<<' [-Wparentheses]`
  - `src\obj_seg000.c:469:29: warning: suggest parentheses around '-' inside '<<' [-Wparentheses]`
  - ? 6 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: add/review `PORT:` coverage for pointer size assumptions; add/review `PORT:` coverage for far pointer normalization; add/review `PORT:` coverage for signed shift

### `src/obj_seg001_complete.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 352 warnings, 20 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- Existing tags: PLATFORM(timer)@742,768,969,1026,1212 (+23); PLATFORM(audio)@968,1419,1421,1790,1834 (+22); PORT: Layout uses MSC /Zp and target scalar widths.@24,33,38,43 (+12); PORT: Signed world-to-map shift is arithmetic; scale is 64:1.@779,780,781,782 (+69); PORT: Q8 value is converted at the legacy boundary.@920,1178,2396,2551 (+3); PORT: Map-to-world scale is 64:1.@1152,1153,1154,1296 (+5); PORT: The MSC unsigned-int product wraps at 16 bits before scaling.@2400; PORT: Far-pointer stepping must preserve DOS segment:offset normalization.@2927,2930,2935
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 351 distinct warning/error lines
  - `src\obj_seg001_complete.c:556:13: warning: conflicting types for built-in function 'exit'; expected 'void(int)' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg001_complete.c:647:12: warning: conflicting types for built-in function 'abs'; expected 'int(int)' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg001_complete.c:685:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - ? 348 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 13 distinct warning/error lines
  - `src\obj_seg001_complete.c:806:27: warning: passing argument 2 of 'track_edge_points' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `src\obj_seg001_complete.c:868:27: warning: passing argument 2 of 'track_edge_points' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `src\obj_seg001_complete.c:1624:30: warning: assignment to 'char *' from incompatible pointer type 'char (*)[62]' [-Wincompatible-pointer-types]`
  - ? 10 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 8 distinct warning/error lines
  - `src\obj_seg001_complete.c:548:41: warning: 'struct SPRITE' declared inside parameter list will not be visible outside of this definition or declaration`
  - `src\obj_seg001_complete.c:633:23: error: array type has incomplete element type 'struct SHAPE3D'`
  - `src\obj_seg001_complete.c:635:29: warning: 'struct TRANSFORMSHAPE3D' declared inside parameter list will not be visible outside of this definition or declaration`
  - ? 5 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: the screened hazards have matching source PORT tags

### `src/obj_seg003.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 253 warnings, 15 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- Existing tags: PLATFORM(video)@762,784,804,809,811 (+93); PLATFORM(file)@2907,2917,2918,2965,2979 (+5); PLATFORM(memory)@3006,3009,3027,3028,3037 (+2); PLATFORM(input_joy)@3034,3220; PLATFORM(input_kb)@3035,3220; PLATFORM(input_mouse)@3036,3220; PLATFORM(timer)@3038,3103,3117; PORT: plain char signedness follows the pinned MSC target.@31; PORT: aggregate field offsets rely on the pinned MSC default /Zp packing.@34,41,45,49 (+18); PORT: negative signed 32-bit world coordinates rely on arithmetic right shift.@946,947,948,953 (+23); PORT: crash-shape data uses a segmented FAR resource pointer.@2917; PORT: animation metadata uses a segmented FAR resource pointer.@2918
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 253 distinct warning/error lines
  - `src\obj_seg003.c:556:13: warning: conflicting types for built-in function 'exit'; expected 'void(int)' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg003.c:693:17: warning: conflicting types for built-in function 'strlen'; expected 'long long unsigned int(const char *)' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg003.c:790:28: warning: conversion from 'int' to 'int16_t' {aka 'short int'} may change value [-Wconversion]`
  - ? 250 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 7 distinct warning/error lines
  - `src\obj_seg003.c:803:31: warning: passing argument 3 of 'rectsorttop' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `src\obj_seg003.c:1726:145: warning: passing argument 3 of 'wheel_update' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `src\obj_seg003.c:1779:147: warning: passing argument 3 of 'wheel_update' from incompatible pointer type [-Wincompatible-pointer-types]`
  - ? 4 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **segment arithmetic:** explicit segment/offset helper or spelling
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 8 distinct warning/error lines
  - `src\obj_seg003.c:548:41: warning: 'struct SPRITE' declared inside parameter list will not be visible outside of this definition or declaration`
  - `src\obj_seg003.c:1701:176: warning: suggest parentheses around '+' inside '>>' [-Wparentheses]`
  - `src\obj_seg003.c:1702:176: warning: suggest parentheses around '+' inside '>>' [-Wparentheses]`
  - ? 5 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: the screened hazards have matching source PORT tags

### `src/obj_seg004.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 123 warnings, 0 errors.
- Existing tags: PLATFORM(file)@975,986,987,1011,1023 (+5); PLATFORM(memory)@976,983,999,1003,1006 (+11); PORT: plain char signedness follows the pinned MSC target.@6; PORT: aggregate field offsets rely on the pinned MSC default /Zp packing.@7,9,11,13 (+4); PORT: short base heights are promoted to 16-bit int for signed division.@1156; PORT: the speed table is traversed through a segmented FAR resource pointer.@1926
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 103 distinct warning/error lines
  - `src\obj_seg004.c:63:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `src\obj_seg004.c:64:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `src\obj_seg004.c:170:26: warning: conversion from 'int' to 'short int' may change value [-Wconversion]`
  - ? 100 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 3 distinct warning/error lines
  - `src\obj_seg004.c:1222:59: warning: assignment to 'struct TrackNode *' from incompatible pointer type 'char *' [-Wincompatible-pointer-types]`
  - `src\obj_seg004.c:1680:19: warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]`
  - `src\obj_seg004.c:1835:19: warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]`
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 17 distinct warning/error lines
  - `src\obj_seg004.c:128:6: warning: no previous prototype for 'build_obj' [-Wmissing-prototypes]`
  - `src\obj_seg004.c:844:1: warning: label 'position_wall' defined but not used [-Wunused-label]`
  - `src\obj_seg004.c:132:14: warning: variable 'angAngle' set but not used [-Wunused-but-set-variable]`
  - ? 14 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: add/review `PORT:` coverage for signed shift

### `src/obj_seg005.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 273 warnings, 1 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- Existing tags: PLATFORM(audio)@638,760,767,901,946 (+23); PLATFORM(file)@639,666,1361,1364,1367 (+24); PLATFORM(input_joy)@640,681,725,761,885 (+34); PLATFORM(input_kb)@641,682,725,759,769 (+27); PLATFORM(input_mouse)@642,683,711,725,759 (+30); PLATFORM(timer)@643,684,762,950,977 (+33); PLATFORM(video)@644,685,763,769,781 (+124); PLATFORM(memory)@1454,1713,1714,1730,1752 (+8); PORT: plain char signedness follows the pinned MSC target.@31; PORT: aggregate field offsets rely on the pinned MSC default /Zp packing.@37,45,50,55 (+11); PORT: negative signed 32-bit world coordinates rely on arithmetic right shift.@1298,1300,1302; PORT: car_speed is unsigned 16-bit Q8 mph; this shift is a zero-fill conversion.@1621
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 267 distinct warning/error lines
  - `src\obj_seg005.c:601:13: warning: conflicting types for built-in function 'exit'; expected 'void(int)' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg005.c:657:19: warning: implicit declaration of function 'get_kevinrandom' [-Wimplicit-function-declaration]`
  - `src\obj_seg005.c:657:19: warning: conversion from 'int' to 'int16_t' {aka 'short int'} may change value [-Wconversion]`
  - ? 264 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 7 distinct warning/error lines
  - `src\obj_seg005.c:61:32: warning: ISO C does not allow extra ';' outside of a function [-Wpedantic]`
  - `src\obj_seg005.c:1063:6: warning: no previous prototype for 'initialize_unknown' [-Wmissing-prototypes]`
  - `src\obj_seg005.c:1269:10: warning: no previous prototype for 'update_camera_target' [-Wmissing-prototypes]`
  - ? 4 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: the screened hazards have matching source PORT tags

### `src/obj_seg006.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 86 warnings, 0 errors.
- Existing tags: PLATFORM(memory)@137,138; PLATFORM(video)@637,667,671,674,679 (+3); PORT: This in-memory record uses MSC medium-model far pointers and default 2-byte packing; verify field offsets before host serialization.@23; PORT: This record contains a near pointer; MSC pointer width and default 2-byte packing determine its offsets.@34; PORT: This record is a 16-bit packed renderer structure; preserve MSC field alignment and signed widths.@44; PORT: signed right shift uses the target arithmetic-shift behavior.@278,279,280,333 (+2)
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 76 distinct warning/error lines
  - `src\obj_seg006.c:54:16: warning: conflicting types for built-in function 'abs'; expected 'int(int)' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg006.c:133:1: warning: type defaults to 'int' in declaration of 'vector_op_unk2' [-Wimplicit-int]`
  - `src\obj_seg006.c:9:33: warning: overflow in conversion from 'int' to 'int16_t' {aka 'short int'} changes value from '65535' to '-1' [-Woverflow]`
  - ? 73 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries; 1 distinct warning/error lines
  - `src\obj_seg006.c:107:11: warning: 'poly_padding_bytes' defined but not used [-Wunused-variable]`
- **other compiler diagnostic:** compiler diagnostic; 9 distinct warning/error lines
  - `src\obj_seg006.c:133:1: warning: data definition has no type or storage class`
  - `src\obj_seg006.c:136:6: warning: no previous prototype for 'initialize_polyinfo' [-Wmissing-prototypes]`
  - `src\obj_seg006.c:146:6: warning: no previous prototype for 'copy_material_list_pointers' [-Wmissing-prototypes]`
  - ? 6 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: the screened hazards have matching source PORT tags

### `src/obj_seg007.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 35 warnings, 3 errors.
- BLOCKER CLASS: 3 (GCC does not accept the legacy inline assembly/compiler dialect in this source).
- Existing tags: PLATFORM(timer)@64,70,74,82; PLATFORM(audio)@74,79,98,119,120 (+46); PLATFORM(file)@98,118
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 10 distinct warning/error lines
  - `src\obj_seg007.c:195:17: warning: conversion from 'u16' {aka 'unsigned int'} to 'u8' {aka 'unsigned char'} may change value [-Wconversion]`
  - `src\obj_seg007.c:268:15: warning: conversion to 'u16' {aka 'unsigned int'} from 'int' may change the sign of the result [-Wsign-conversion]`
  - `src\obj_seg007.c:288:99: warning: conversion to 'int' from 'u16' {aka 'unsigned int'} may change the sign of the result [-Wsign-conversion]`
  - ? 7 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **inline asm:** inline assembly token; 1 distinct warning/error lines
  - `src\obj_seg007.c:250:5: error: '_asm' undeclared (first use in this function)`
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 27 distinct warning/error lines
  - `src\obj_seg007.c:63:10: warning: no previous prototype for 'audio_add_driver_timer' [-Wmissing-prototypes]`
  - `src\obj_seg007.c:73:10: warning: no previous prototype for 'audio_remove_driver_timer' [-Wmissing-prototypes]`
  - `src\obj_seg007.c:85:12: warning: no previous prototype for 'pad_id' [-Wmissing-prototypes]`
  - ? 24 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: add/review `PORT:` coverage for implicit int widths; add/review `PORT:` coverage for inline asm; add/review `PORT:` coverage for far pointer normalization; add/review `PORT:` coverage for signed shift; add/review `PORT:` coverage for struct packing

### `src/obj_seg008.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 229 warnings, 44 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- Existing tags: PLATFORM(memory)@143,159,896,897,1440 (+2); PLATFORM(video)@143,161,162,165,168 (+211); PLATFORM(timer)@207,372,376,389,429 (+21); PLATFORM(input_kb)@207,376,429,487,611 (+48); PLATFORM(input_mouse)@207,430,487,612,782 (+1); PLATFORM(file)@487,520,548,561,580 (+59); PLATFORM(input_joy)@782,795,1588,1637,1640; PLATFORM(audio)@1429,1433,1434,1440,1441 (+22); PLATFORM(dos)@1760,1771; PORT: MSC ctype lookup assumes an 8-bit character code; negative plain-char values can index before _ctype+1 on a host.@15; PORT: These game records rely on 16-bit pointers and MSC default 2-byte field alignment.@31; PORT: This renderer record stores a far pointer; pointer width and default packing are part of the target ABI.@33; PORT: Resource headers are read directly; preserve byte fields and 2-byte record alignment.@50; PORT: This resource view contains several far pointers and uses the MSC medium-model layout.@52; PORT: This compact snapshot is an MSC 16-bit record; host widths and alignment must not alter its byte layout.@71; PORT: advancing this resource source uses the original far-pointer offset semantics.@925; PORT: 16-bit signed products, left shifts, and division determine the slider thumb position.@958,1027; PORT: far-pointer arithmetic follows 16-bit segment:offset rules and the encoded resource record sizes.@1317; PORT: resource offsets must retain 16-bit far-pointer arithmetic and exact record sizes.@1318,1318,1320,1322
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 193 distinct warning/error lines
  - `src\obj_seg008.c:22:17: warning: conflicting types for built-in function 'strlen'; expected 'long long unsigned int(const char *)' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg008.c:23:12: warning: mismatch in argument 2 type of built-in function 'strcpy'; expected 'const char *' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg008.c:24:12: warning: mismatch in argument 2 type of built-in function 'strcat'; expected 'const char *' [-Wbuiltin-declaration-mismatch]`
  - ? 190 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 15 distinct warning/error lines
  - `src\obj_seg008.c:820:21: warning: passing argument 1 of 'mouse_get_state' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `src\obj_seg008.c:820:33: warning: passing argument 2 of 'mouse_get_state' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `src\obj_seg008.c:820:45: warning: passing argument 3 of 'mouse_get_state' from incompatible pointer type [-Wincompatible-pointer-types]`
  - ? 12 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer; 12 distinct warning/error lines
  - `src\obj_seg008.c:1431:43: warning: passing argument 2 of 'file_load_resource' discards 'const' qualifier from pointer target type [-Wdiscarded-qualifiers]`
  - `src\obj_seg008.c:1432:43: warning: passing argument 2 of 'file_load_resource' discards 'const' qualifier from pointer target type [-Wdiscarded-qualifiers]`
  - `src\obj_seg008.c:1433:63: warning: passing argument 3 of 'init_audio_resources' discards 'const' qualifier from pointer target type [-Wdiscarded-qualifiers]`
  - ? 9 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 65 distinct warning/error lines
  - `src\obj_seg008.c:142:8: warning: no previous prototype for 'point_in_rectangle' [-Wmissing-prototypes]`
  - `src\obj_seg008.c:179:10: warning: no previous prototype for 'restore_mouse_sprite' [-Wmissing-prototypes]`
  - `src\obj_seg008.c:205:9: warning: no previous prototype for 'show_dialog' [-Wmissing-prototypes]`
  - ? 62 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: the screened hazards have matching source PORT tags

### `src/obj_seg009.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 124 warnings, 2 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- Existing tags: PLATFORM(input_kb)@166,178,515; PLATFORM(file)@178,238,239,240,241 (+32); PLATFORM(video)@178,243,245,247,249 (+96); PLATFORM(input_mouse)@178,370,372,384,385 (+4); PLATFORM(timer)@178,513,607; PLATFORM(memory)@178,927,928; PORT: Resource records contain far pointers and depend on MSC default 2-byte packing; never use the host layout as the file layout.@32; PORT: Track records use 16-bit near pointers and MSC default structure alignment; keep verified offsets.@48; PORT: Game-info records are byte/word layouts from the original 16-bit executable; preserve packing and widths.@61; PORT: signed 8-bit mapRow is promoted to 16-bit int; keep the target row-major index width.@982; PORT: this index arithmetic is 16-bit int over a far resource table; preserve segment-bounded access.@984; PORT: 0xff is the byte-cache invalid sentinel, distinct from the serialized continuation codes.@989,1057
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 117 distinct warning/error lines
  - `src\obj_seg009.c:116:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `src\obj_seg009.c:117:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `src\obj_seg009.c:121:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - ? 114 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 4 distinct warning/error lines
  - `src\obj_seg009.c:239:70: warning: passing argument 3 of 'locate_many_resources' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `src\obj_seg009.c:262:17: warning: pointer targets in assignment from 'uint8_t *' {aka 'unsigned char *'} to 'char *' differ in signedness [-Wpointer-sign]`
  - `src\obj_seg009.c:268:17: warning: pointer targets in assignment from 'uint8_t *' {aka 'unsigned char *'} to 'char *' differ in signedness [-Wpointer-sign]`
  - ? 1 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 5 distinct warning/error lines
  - `src\obj_seg009.c:177:6: warning: no previous prototype for 'load_tracks_menu_shapes' [-Wmissing-prototypes]`
  - `src\obj_seg009.c:931:6: warning: no previous prototype for 'preRender_icons' [-Wmissing-prototypes]`
  - `src\obj_seg009.c:972:6: warning: no previous prototype for 'draw_2DtrackMap' [-Wmissing-prototypes]`
  - ? 2 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: add/review `PORT:` coverage for signed shift

### `src/obj_seg016_group.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 4 warnings, 0 errors.
- Existing tags: PLATFORM(file)@6,8,14,18,26 (+3)
- **other compiler diagnostic:** compiler diagnostic; 4 distinct warning/error lines
  - `src\obj_seg016_group.c:7:6: warning: no previous prototype for 'locate_many_resources' [-Wmissing-prototypes]`
  - `src\obj_seg016_group.c:15:10: warning: no previous prototype for 'nopsub_367E4' [-Wmissing-prototypes]`
  - `src\obj_seg016_group.c:27:10: warning: no previous prototype for 'nopsub_36826' [-Wmissing-prototypes]`
  - ? 1 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: the screened hazards have matching source PORT tags

### `src/obj_seg027.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 67 warnings, 1 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- Existing tags: PLATFORM(file)@91,93,108,509,527 (+9); PLATFORM(memory)@95,532,570; PLATFORM(timer)@99,101,104,106,421 (+10); PLATFORM(audio)@171,181,196,213,219 (+17); PORT: Packed audio records depend on 16-bit far-pointer width and MSC field packing.@16; PORT: This offset selects a driver entry inside a loaded segment; preserve 16:16 pointer semantics.@182
- **implicit int widths:** compiler diagnostic; 31 distinct warning/error lines
  - `src\obj_seg027.c:115:5: warning: conflicting types for built-in function 'strlen'; expected 'long long unsigned int(const char *)' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg027.c:130:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `src\obj_seg027.c:183:6: warning: ISO C forbids conversion of object pointer to function pointer type [-Wpedantic]`
  - ? 28 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 2 distinct warning/error lines
  - `src\obj_seg027.c:708:28: warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]`
  - `src\obj_seg027.c:709:27: warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]`
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** explicit packing/alignment directive; aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 35 distinct warning/error lines
  - `src\obj_seg027.c:132:16: warning: no previous prototype for 'init_audio_resources' [-Wmissing-prototypes]`
  - `src\obj_seg027.c:172:10: warning: no previous prototype for 'load_audio_finalize' [-Wmissing-prototypes]`
  - `src\obj_seg027.c:197:10: warning: no previous prototype for 'audio_unk' [-Wmissing-prototypes]`
  - ? 32 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: add/review `PORT:` coverage for signed shift

### `src/obj_seg028.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 51 warnings, 0 errors.
- Existing tags: PLATFORM(audio)@156,187,219,222,293 (+36); PORT: Runtime audio records contain 16-bit far pointers; host pointer width changes their layout.@11
- **implicit int widths:** compiler diagnostic; 42 distinct warning/error lines
  - `src\obj_seg028.c:188:22: warning: ISO C forbids conversion of object pointer to function pointer type [-Wpedantic]`
  - `src\obj_seg028.c:216:52: warning: conversion to 'uint8_t' {aka 'unsigned char'} from 'char' may change the sign of the result [-Wsign-conversion]`
  - `src\obj_seg028.c:220:26: warning: ISO C forbids conversion of object pointer to function pointer type [-Wpedantic]`
  - ? 39 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 3 distinct warning/error lines
  - `src\obj_seg028.c:169:48: warning: pointer targets in passing argument 2 of 'read_audio_event' differ in signedness [-Wpointer-sign]`
  - `src\obj_seg028.c:197:34: warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]`
  - `src\obj_seg028.c:272:65: warning: pointer targets in passing argument 2 of 'read_audio_event' differ in signedness [-Wpointer-sign]`
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 6 distinct warning/error lines
  - `src\obj_seg028.c:113:18: warning: no previous prototype for 'audiodriver_timer' [-Wmissing-prototypes]`
  - `src\obj_seg028.c:366:18: warning: no previous prototype for 'start_audio_voice_sample' [-Wmissing-prototypes]`
  - `src\obj_seg028.c:489:18: warning: no previous prototype for 'send_audio_stop_event' [-Wmissing-prototypes]`
  - ? 3 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: add/review `PORT:` coverage for signed shift

### `src/obj_seg029.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 8 warnings, 0 errors.
- Existing tags: PORT: Huge-pointer addition must preserve segment:offset normalization within this resource.@61
- **implicit int widths:** compiler diagnostic; 4 distinct warning/error lines
  - `src\obj_seg029.c:3:16: warning: conflicting types for built-in function 'toupper'; expected 'int(int)' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg029.c:39:31: warning: conversion to 'uint8_t' {aka 'unsigned char'} from 'char' may change the sign of the result [-Wsign-conversion]`
  - `src\obj_seg029.c:59:46: warning: conversion to 'int16_t' {aka 'short int'} from 'uint16_t' {aka 'short unsigned int'} may change the sign of the result [-Wsign-conversion]`
  - ? 1 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **other compiler diagnostic:** compiler diagnostic; 4 distinct warning/error lines
  - `src\obj_seg029.c:8:9: warning: no previous prototype for 'audioresource_compare_chunknames' [-Wmissing-prototypes]`
  - `src\obj_seg029.c:29:9: warning: no previous prototype for 'audioresource_get_chunk_index' [-Wmissing-prototypes]`
  - `src\obj_seg029.c:49:14: warning: no previous prototype for 'audioresource_find' [-Wmissing-prototypes]`
  - ? 1 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: the screened hazards have matching source PORT tags

### `src/obj_seg031.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 21 warnings, 0 errors.
- Existing tags: PLATFORM(file)@15,17,19,24,28 (+11); PLATFORM(input_kb)@56,58,60,62,137 (+12); PLATFORM(memory)@72,119,185,373,398 (+7); PLATFORM(video)@74,76,80,96,98 (+30); PLATFORM(timer)@78,100,140,239,275 (+3); PLATFORM(input_mouse)@82,141,245; PLATFORM(audio)@84,86,89,91,142 (+4); PLATFORM(dos)@138,253,264; PLATFORM(bios)@361
- **implicit int widths:** compiler diagnostic; 11 distinct warning/error lines
  - `src\obj_seg031.c:88:17: warning: conflicting types for built-in function 'exit'; expected 'void(int)' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg031.c:108:16: warning: conflicting types for built-in function 'strlen'; expected 'long long unsigned int(const char *)' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg031.c:206:40: warning: array subscript has type 'char' [-Wchar-subscripts]`
  - ? 8 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 10 distinct warning/error lines
  - `src\obj_seg031.c:16:11: warning: no previous prototype for 'file_load_shape2d_nofatal2' [-Wmissing-prototypes]`
  - `src\obj_seg031.c:25:10: warning: no previous prototype for 'file_combine_and_find' [-Wmissing-prototypes]`
  - `src\obj_seg031.c:39:11: warning: no previous prototype for 'file_find_next_alt' [-Wmissing-prototypes]`
  - ? 7 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: add/review `PORT:` coverage for implicit int widths; add/review `PORT:` coverage for far pointer normalization; add/review `PORT:` coverage for signed shift; add/review `PORT:` coverage for struct packing

### `src/obj_seg032_group.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 5 warnings, 0 errors.
- Existing tags: PLATFORM(video)@17,20,22,24,44 (+11); PLATFORM(input_kb)@29,31,43,91,93; PLATFORM(timer)@33,35,37,45,77 (+4)
- **implicit int widths:** compiler diagnostic; 2 distinct warning/error lines
  - `src\obj_seg032_group.c:16:12: warning: conflicting types for built-in function 'strlen'; expected 'long long unsigned int(const char *)' [-Wbuiltin-declaration-mismatch]`
  - `src\obj_seg032_group.c:280:9: warning: conversion from 'int' to 'int16_t' {aka 'short int'} may change value [-Wconversion]`
- **segment arithmetic:** explicit segment/offset helper or spelling
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 3 distinct warning/error lines
  - `src\obj_seg032_group.c:46:9: warning: no previous prototype for 'read_line' [-Wmissing-prototypes]`
  - `src\obj_seg032_group.c:124:125: warning: suggest parentheses around '&&' within '||' [-Wparentheses]`
  - `src\obj_seg032_group.c:124:170: warning: suggest parentheses around '&&' within '||' [-Wparentheses]`
- Tagging follow-up: add/review `PORT:` coverage for implicit int widths; add/review `PORT:` coverage for segment arithmetic; add/review `PORT:` coverage for struct packing

### `src/obj_seg035_group.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 11 warnings, 0 errors.
- Existing tags: PLATFORM(memory)@6,10,12,14,16 (+9); PLATFORM(file)@8,30,32,38,40 (+5); PORT: Resource headers use MSC 16-bit int layout; assert these field offsets in a host port.@3
- **implicit int widths:** compiler diagnostic; 8 distinct warning/error lines
  - `src\obj_seg035_group.c:62:21: warning: conversion to 'int16_t' {aka 'short int'} from 'uint16_t' {aka 'short unsigned int'} may change the sign of the result [-Wsign-conversion]`
  - `src\obj_seg035_group.c:64:43: warning: conversion to 'uint16_t' {aka 'short unsigned int'} from 'int16_t' {aka 'short int'} may change the sign of the result [-Wsign-conversion]`
  - `src\obj_seg035_group.c:98:13: warning: conversion to 'int16_t' {aka 'short int'} from 'uint16_t' {aka 'short unsigned int'} may change the sign of the result [-Wsign-conversion]`
  - ? 5 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 3 distinct warning/error lines
  - `src\obj_seg035_group.c:31:11: warning: no previous prototype for 'file_load_shape2d_res_fatal' [-Wmissing-prototypes]`
  - `src\obj_seg035_group.c:39:11: warning: no previous prototype for 'file_load_shape2d_res_nofatal' [-Wmissing-prototypes]`
  - `src\obj_seg035_group.c:92:9: warning: unused variable 'baseOff' [-Wunused-variable]`
- Tagging follow-up: add/review `PORT:` coverage for signed shift

### `src/polarRadius3D.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- Existing tags: none
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\polarRadius3D.c:7:5: warning: no previous prototype for 'polarRadius3D' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for struct packing

### `src/preRender_sphere_helper.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- Existing tags: PLATFORM(video)@10,15
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\preRender_sphere_helper.c:11:6: warning: no previous prototype for 'preRender_sphere_helper' [-Wmissing-prototypes]`
- Tagging follow-up: the screened hazards have matching source PORT tags

### `src/preRender_sphere_helper2.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- Existing tags: PORT: Signed geometry coordinates use arithmetic right shifts when halved.@24
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\preRender_sphere_helper2.c:10:10: warning: no previous prototype for 'preRender_sphere_helper2' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for struct packing

### `src/preRender_wheel.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- Existing tags: PLATFORM(video)@17,36,44,81,103 (+1)
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\preRender_wheel.c:18:10: warning: no previous prototype for 'preRender_wheel' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for struct packing

### `src/preRender_wheel_helper.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- Existing tags: none
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\preRender_wheel_helper.c:11:10: warning: no previous prototype for 'preRender_wheel_helper' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for struct packing

### `src/preRender_wheel_helper2.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- Existing tags: none
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\preRender_wheel_helper2.c:11:6: warning: no previous prototype for 'preRender_wheel_helper2' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for struct packing

### `src/preRender_wheel_helper3.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- Existing tags: PORT: Signed geometry coordinates use arithmetic right shifts when halved.@27
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\preRender_wheel_helper3.c:12:6: warning: no previous prototype for 'preRender_wheel_helper3' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for struct packing

### `src/seg017_mouse_whole.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 13 warnings, 0 errors.
- Existing tags: PLATFORM(input_mouse)@30,36,43,51,68 (+16); PLATFORM(bios)@48; PORT: INT register packets require 16-bit word fields and MSC-compatible struct layout.@5
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 5 distinct warning/error lines
  - `src\seg017_mouse_whole.c:60:36: warning: overflow in conversion from 'int' to 'int16_t' {aka 'short int'} changes value from '65535' to '-1' [-Woverflow]`
  - `src\seg017_mouse_whole.c:62:12: warning: conversion to 'int16_t' {aka 'short int'} from 'uint16_t' {aka 'short unsigned int'} may change the sign of the result [-Wsign-conversion]`
  - `src\seg017_mouse_whole.c:96:12: warning: conversion from 'unsigned int' to 'uint16_t' {aka 'short unsigned int'} may change value [-Wconversion]`
  - ? 2 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **int86/port I/O:** DOS interrupt, port-I/O intrinsic, or intrinsic pragma
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 8 distinct warning/error lines
  - `src\seg017_mouse_whole.c:44:9: warning: no previous prototype for 'mouse_init' [-Wmissing-prototypes]`
  - `src\seg017_mouse_whole.c:130:10: warning: no previous prototype for 'mouse_set_position' [-Wmissing-prototypes]`
  - `src\seg017_mouse_whole.c:145:10: warning: no previous prototype for 'mouse_get_state' [-Wmissing-prototypes]`
  - ? 5 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- Tagging follow-up: add/review `PORT:` coverage for int86/port I/O; add/review `PORT:` coverage for signed shift

### `src/seg024_matrot.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 3 warnings, 0 errors.
- Existing tags: none
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 3 distinct warning/error lines
  - `src\seg024_matrot.c:12:6: warning: no previous prototype for 'mat_rot_x' [-Wmissing-prototypes]`
  - `src\seg024_matrot.c:24:6: warning: no previous prototype for 'matroty' [-Wmissing-prototypes]`
  - `src\seg024_matrot.c:36:6: warning: no previous prototype for 'mat_rot_z' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for struct packing

### `src/seg033_mcgawnd.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 2 warnings, 0 errors.
- Existing tags: PLATFORM(video)@12,14,16,22,26 (+5)
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 2 distinct warning/error lines
  - `src\seg033_mcgawnd.c:23:10: warning: no previous prototype for 'setup_mcgawnd1' [-Wmissing-prototypes]`
  - `src\seg033_mcgawnd.c:38:10: warning: no previous prototype for 'setup_mcgawnd2' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for far pointer normalization; add/review `PORT:` coverage for struct packing

### `src/seg034_shape2d_group.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 2 warnings, 0 errors.
- Existing tags: PLATFORM(memory)@22,29,35,46,73 (+9); PLATFORM(file)@24,26,31,33,37 (+22)
- **other compiler diagnostic:** compiler diagnostic; 2 distinct warning/error lines
  - `src\seg034_shape2d_group.c:55:11: warning: no previous prototype for 'file_load_shape2d_fatal' [-Wmissing-prototypes]`
  - `src\seg034_shape2d_group.c:64:11: warning: no previous prototype for 'file_load_shape2d_nofatal' [-Wmissing-prototypes]`
- Tagging follow-up: the screened hazards have matching source PORT tags

### `src/sprite_1_unk4.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 2 warnings, 0 errors.
- Existing tags: PLATFORM(video)@7,15,17,21,23
- **implicit int widths:** compiler diagnostic; 1 distinct warning/error lines
  - `src\sprite_1_unk4.c:12:13: warning: conversion from 'int' to 'int16_t' {aka 'short int'} may change value [-Wconversion]`
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\sprite_1_unk4.c:8:10: warning: no previous prototype for 'sprite_1_unk4' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for implicit int widths

### `src/sub_3702E.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 3 warnings, 0 errors.
- Existing tags: PLATFORM(video)@8,18,20,25,27
- **implicit int widths:** compiler diagnostic; 2 distinct warning/error lines
  - `src\sub_3702E.c:14:13: warning: conversion from 'int' to 'int16_t' {aka 'short int'} may change value [-Wconversion]`
  - `src\sub_3702E.c:15:14: warning: conversion from 'int' to 'int16_t' {aka 'short int'} may change value [-Wconversion]`
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\sub_3702E.c:9:6: warning: no previous prototype for 'draw_rect_outline' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for implicit int widths

### `src/toupper.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 2 warnings, 0 errors.
- Existing tags: none
- **implicit int widths:** compiler diagnostic; 1 distinct warning/error lines
  - `src\toupper.c:5:5: warning: conflicting types for built-in function 'toupper'; expected 'int(int)' [-Wbuiltin-declaration-mismatch]`
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `src\toupper.c:5:5: warning: no previous prototype for 'toupper' [-Wmissing-prototypes]`
- Tagging follow-up: add/review `PORT:` coverage for implicit int widths

### `src/track_constants_module.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 0 warnings, 0 errors.
- Existing tags: none
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- Tagging follow-up: add/review `PORT:` coverage for struct packing

## Category interpretation

- **implicit int widths:** audit bare `int`, implicit declarations/returns, narrowing, and sign conversions against the 16-bit MSC ABI.
- **pointer size assumptions:** audit integer/pointer casts, stored pointer widths, pointer serialization, and pointer argument type mismatches against the 16:16 target ABI.
- **segment arithmetic:** replace segment:offset construction or BIOS/DOS memory addressing with explicit host buffers/handles.
- **inline asm:** port instruction blocks to host C/platform APIs or isolated host assembly.
- **int86/port I/O:** move BIOS/DOS interrupt and direct port services behind SDL/platform service boundaries.
- **far pointer normalization:** erasing `far/near/huge` does not preserve segmented bounds, normalization, or pointer width; review each memory/API boundary.
- **signed shift:** make negative right-shift and signed overflow behavior explicit where output depends on it.
- **struct packing:** compare field widths, alignment, and serialized/on-disk/on-wire layouts; add explicit host-side conversion/layout only when required.
- **other compiler diagnostic:** compiler warning/error did not match a porting category; inspect its exact location in the raw results.

## Reproduction

Run `python tools/porting/host_probe.py --mode legacy` from the repository root. Detailed commands, raw diagnostics, and per-category evidence are in `build/porting/host-probe/legacy/host/results.json`.
