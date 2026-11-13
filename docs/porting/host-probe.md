# Host compiler probe: porting hazard proposal

The 38-source report below is the `--mode legacy` shim survey. Platform-service kinds formerly attached to matching-source comments are stored in `tools/porting/platform-services.json`; the raw probe output records their per-function sets. Port-boundary notes are maintained in the porting docs and semantic contracts. The declaration audit has two port-only modes. After the port-only declaration/adapter fix, both `python tools/porting/host_probe.py --mode compat` and `--mode strict-central` compile 38/38 TUs for syntax and object output. The integ55 freeze-gate receipt preserves the earlier strict-central count of 36/38. See [port-only headers and TU audit](port-headers.md) for the interface changes and remaining runtime residuals. These probes do not link or execute the game.

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

### `lib/audiofn.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 3 warnings, 0 errors.
- **implicit int widths:** compiler diagnostic; 2 distinct warning/error lines
  - `lib\audiofn.c:4:12: warning: conflicting types for built-in function 'strrchr'; expected 'char *(const char *, int)' [-Wbuiltin-declaration-mismatch]`
  - `lib\audiofn.c:7:12: warning: conflicting types for built-in function 'strlen'; expected 'long long unsigned int(const char *)' [-Wbuiltin-declaration-mismatch]`
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\audiofn.c:11:5: warning: no previous prototype for 'audio_make_filename' [-Wmissing-prototypes]`

### `lib/planes.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit)
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer; 1 distinct warning/error lines
  - `lib\planes.c:11:8: warning: missing initializer for field 'plane_rotation' of 'struct PLANE' [-Wmissing-field-initializers]`
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries

### `lib/deffont.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 0 warnings, 0 errors.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer

### `lib/unflipsz.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 2 warnings, 0 errors.
- **implicit int widths:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\unflipsz.c:32:16: warning: conversion from 'int' to 'uint16_t' {aka 'short unsigned int'} may change value [-Wconversion]`
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\unflipsz.c:22:9: warning: no previous prototype for 'file_get_unflip_size' [-Wmissing-prototypes]`

### `lib/expandsz.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 6 warnings, 0 errors.
- **implicit int widths:** compiler diagnostic; 5 distinct warning/error lines
  - `lib\expandsz.c:20:27: warning: conversion to 'long long unsigned int' from 'int' may change the sign of the result [-Wsign-conversion]`
  - `lib\expandsz.c:23:18: warning: conversion from 'int' to 'uint16_t' {aka 'short unsigned int'} may change value [-Wconversion]`
  - `lib\expandsz.c:25:14: warning: conversion to 'long long unsigned int' from 'int32_t' {aka 'int'} may change the sign of the result [-Wsign-conversion]`
  - ? 2 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\expandsz.c:14:9: warning: no previous prototype for 'file_load_shape2d_expandedsize' [-Wmissing-prototypes]`

### `lib/heapsort.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\heapsort.c:5:6: warning: no previous prototype for 'shellsort' [-Wmissing-prototypes]`

### `lib/mousehk.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\mousehk.c:5:10: warning: no previous prototype for 'kb_line_edit_noop' [-Wmissing-prototypes]`

### `game/frontend.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 150 warnings, 2 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 139 distinct warning/error lines
  - `game\frontend.c:140:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `game\frontend.c:156:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `game\frontend.c:163:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - ? 136 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 4 distinct warning/error lines
  - `game\frontend.c:511:10: warning: pointer targets in assignment from 'uint8_t *' {aka 'unsigned char *'} to 'char *' differ in signedness [-Wpointer-sign]`
  - `game\frontend.c:552:87: warning: pointer targets in passing argument 2 of 'loadfileat' differ in signedness [-Wpointer-sign]`
  - `game\frontend.c:761:92: warning: passing argument 3 of 'locateshapes' from incompatible pointer type [-Wincompatible-pointer-types]`
  - ? 1 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 9 distinct warning/error lines
  - `game\frontend.c:456:5: warning: first argument of 'main' should be 'int' [-Wmain]`
  - `game\frontend.c:468:22: warning: suggest parentheses around '-' inside '<<' [-Wparentheses]`
  - `game\frontend.c:469:29: warning: suggest parentheses around '-' inside '<<' [-Wparentheses]`
  - ? 6 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `game/carsim.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 352 warnings, 20 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 351 distinct warning/error lines
  - `game\carsim.c:556:13: warning: conflicting types for built-in function 'exit'; expected 'void(int)' [-Wbuiltin-declaration-mismatch]`
  - `game\carsim.c:647:12: warning: conflicting types for built-in function 'abs'; expected 'int(int)' [-Wbuiltin-declaration-mismatch]`
  - `game\carsim.c:685:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - ? 348 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 13 distinct warning/error lines
  - `game\carsim.c:806:27: warning: passing argument 2 of 'gettlistpoint' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `game\carsim.c:868:27: warning: passing argument 2 of 'gettlistpoint' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `game\carsim.c:1624:30: warning: assignment to 'char *' from incompatible pointer type 'char (*)[62]' [-Wincompatible-pointer-types]`
  - ? 10 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 8 distinct warning/error lines
  - `game\carsim.c:548:41: warning: 'struct SPRITE' declared inside parameter list will not be visible outside of this definition or declaration`
  - `game\carsim.c:633:23: error: array type has incomplete element type 'struct SHAPE3D'`
  - `game\carsim.c:635:29: warning: 'struct TRANSFORMSHAPE3D' declared inside parameter list will not be visible outside of this definition or declaration`
  - ? 5 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `game/render.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 253 warnings, 15 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 253 distinct warning/error lines
  - `game\render.c:556:13: warning: conflicting types for built-in function 'exit'; expected 'void(int)' [-Wbuiltin-declaration-mismatch]`
  - `game\render.c:693:17: warning: conflicting types for built-in function 'strlen'; expected 'long long unsigned int(const char *)' [-Wbuiltin-declaration-mismatch]`
  - `game\render.c:790:28: warning: conversion from 'int' to 'int16_t' {aka 'short int'} may change value [-Wconversion]`
  - ? 250 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 7 distinct warning/error lines
  - `game\render.c:803:31: warning: passing argument 3 of 'sortzonelist' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `game\render.c:1726:145: warning: passing argument 3 of 'settires' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `game\render.c:1779:147: warning: passing argument 3 of 'settires' from incompatible pointer type [-Wincompatible-pointer-types]`
  - ? 4 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **segment arithmetic:** explicit segment/offset helper or spelling
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 8 distinct warning/error lines
  - `game\render.c:548:41: warning: 'struct SPRITE' declared inside parameter list will not be visible outside of this definition or declaration`
  - `game\render.c:1701:176: warning: suggest parentheses around '+' inside '>>' [-Wparentheses]`
  - `game\render.c:1702:176: warning: suggest parentheses around '+' inside '>>' [-Wparentheses]`
  - ? 5 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `game/trkscene.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 123 warnings, 0 errors.
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 103 distinct warning/error lines
  - `game\trkscene.c:63:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `game\trkscene.c:64:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `game\trkscene.c:170:26: warning: conversion from 'int' to 'short int' may change value [-Wconversion]`
  - ? 100 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 3 distinct warning/error lines
  - `game\trkscene.c:1222:59: warning: assignment to 'struct TrackNode *' from incompatible pointer type 'char *' [-Wincompatible-pointer-types]`
  - `game\trkscene.c:1680:19: warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]`
  - `game\trkscene.c:1835:19: warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]`
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 17 distinct warning/error lines
  - `game\trkscene.c:128:6: warning: no previous prototype for 'getpinfo' [-Wmissing-prototypes]`
  - `game\trkscene.c:844:1: warning: label 'position_wall' defined but not used [-Wunused-label]`
  - `game\trkscene.c:132:14: warning: variable 'angAngle' set but not used [-Wunused-but-set-variable]`
  - ? 14 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `game/race.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 273 warnings, 1 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 267 distinct warning/error lines
  - `game\race.c:601:13: warning: conflicting types for built-in function 'exit'; expected 'void(int)' [-Wbuiltin-declaration-mismatch]`
  - `game\race.c:657:19: warning: implicit declaration of function 'myrandom' [-Wimplicit-function-declaration]`
  - `game\race.c:657:19: warning: conversion from 'int' to 'int16_t' {aka 'short int'} may change value [-Wconversion]`
  - ? 264 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 7 distinct warning/error lines
  - `game\race.c:61:32: warning: ISO C does not allow extra ';' outside of a function [-Wpedantic]`
  - `game\race.c:1063:6: warning: no previous prototype for 'initworld' [-Wmissing-prototypes]`
  - `game\race.c:1269:10: warning: no previous prototype for 'movehelicopters' [-Wmissing-prototypes]`
  - ? 4 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `game/polygon.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 86 warnings, 0 errors.
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 76 distinct warning/error lines
  - `game\polygon.c:54:16: warning: conflicting types for built-in function 'abs'; expected 'int(int)' [-Wbuiltin-declaration-mismatch]`
  - `game\polygon.c:133:1: warning: type defaults to 'int' in declaration of 'norm_zone' [-Wimplicit-int]`
  - `game\polygon.c:9:33: warning: overflow in conversion from 'int' to 'int16_t' {aka 'short int'} changes value from '65535' to '-1' [-Woverflow]`
  - ? 73 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries; 1 distinct warning/error lines
  - `game\polygon.c:107:11: warning: 'poly_padding_bytes' defined but not used [-Wunused-variable]`
- **other compiler diagnostic:** compiler diagnostic; 9 distinct warning/error lines
  - `game\polygon.c:133:1: warning: data definition has no type or storage class`
  - `game\polygon.c:136:6: warning: no previous prototype for 'INIT_MATRIX' [-Wmissing-prototypes]`
  - `game\polygon.c:146:6: warning: no previous prototype for 'INIT_PALETTE' [-Wmissing-prototypes]`
  - ? 6 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `game/audioeng.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 35 warnings, 3 errors.
- BLOCKER CLASS: 3 (GCC does not accept the legacy inline assembly/compiler dialect in this source).
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 10 distinct warning/error lines
  - `game\audioeng.c:195:17: warning: conversion from 'u16' {aka 'unsigned int'} to 'u8' {aka 'unsigned char'} may change value [-Wconversion]`
  - `game\audioeng.c:268:15: warning: conversion to 'u16' {aka 'unsigned int'} from 'int' may change the sign of the result [-Wsign-conversion]`
  - `game\audioeng.c:288:99: warning: conversion to 'int' from 'u16' {aka 'unsigned int'} may change the sign of the result [-Wsign-conversion]`
  - ? 7 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **inline asm:** inline assembly token; 1 distinct warning/error lines
  - `game\audioeng.c:250:5: error: '_asm' undeclared (first use in this function)`
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 27 distinct warning/error lines
  - `game\audioeng.c:63:10: warning: no previous prototype for 'audio_add_driver_timer' [-Wmissing-prototypes]`
  - `game\audioeng.c:73:10: warning: no previous prototype for 'audio_remove_driver_timer' [-Wmissing-prototypes]`
  - `game\audioeng.c:85:12: warning: no previous prototype for 'pad_id' [-Wmissing-prototypes]`
  - ? 24 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `game/uires.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 229 warnings, 44 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 193 distinct warning/error lines
  - `game\uires.c:22:17: warning: conflicting types for built-in function 'strlen'; expected 'long long unsigned int(const char *)' [-Wbuiltin-declaration-mismatch]`
  - `game\uires.c:23:12: warning: mismatch in argument 2 type of built-in function 'strcpy'; expected 'const char *' [-Wbuiltin-declaration-mismatch]`
  - `game\uires.c:24:12: warning: mismatch in argument 2 type of built-in function 'strcat'; expected 'const char *' [-Wbuiltin-declaration-mismatch]`
  - ? 190 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 15 distinct warning/error lines
  - `game\uires.c:820:21: warning: passing argument 1 of 'getmousepos' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `game\uires.c:820:33: warning: passing argument 2 of 'getmousepos' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `game\uires.c:820:45: warning: passing argument 3 of 'getmousepos' from incompatible pointer type [-Wincompatible-pointer-types]`
  - ? 12 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer; 12 distinct warning/error lines
  - `game\uires.c:1431:43: warning: passing argument 2 of 'myload' discards 'const' qualifier from pointer target type [-Wdiscarded-qualifiers]`
  - `game\uires.c:1432:43: warning: passing argument 2 of 'myload' discards 'const' qualifier from pointer target type [-Wdiscarded-qualifiers]`
  - `game\uires.c:1433:63: warning: passing argument 3 of 'init_audio_resources' discards 'const' qualifier from pointer target type [-Wdiscarded-qualifiers]`
  - ? 9 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 65 distinct warning/error lines
  - `game\uires.c:142:8: warning: no previous prototype for 'point_in_rectangle' [-Wmissing-prototypes]`
  - `game\uires.c:179:10: warning: no previous prototype for 'restoreboxwindow' [-Wmissing-prototypes]`
  - `game\uires.c:205:9: warning: no previous prototype for 'messagebox' [-Wmissing-prototypes]`
  - ? 62 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `game/trackedt.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 124 warnings, 2 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 117 distinct warning/error lines
  - `game\trackedt.c:116:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `game\trackedt.c:117:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `game\trackedt.c:121:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - ? 114 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 4 distinct warning/error lines
  - `game\trackedt.c:239:70: warning: passing argument 3 of 'locateshapes' from incompatible pointer type [-Wincompatible-pointer-types]`
  - `game\trackedt.c:262:17: warning: pointer targets in assignment from 'uint8_t *' {aka 'unsigned char *'} to 'char *' differ in signedness [-Wpointer-sign]`
  - `game\trackedt.c:268:17: warning: pointer targets in assignment from 'uint8_t *' {aka 'unsigned char *'} to 'char *' differ in signedness [-Wpointer-sign]`
  - ? 1 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 5 distinct warning/error lines
  - `game\trackedt.c:177:6: warning: no previous prototype for 'trackedit' [-Wmissing-prototypes]`
  - `game\trackedt.c:931:6: warning: no previous prototype for 'drawpiecebox' [-Wmissing-prototypes]`
  - `game\trackedt.c:972:6: warning: no previous prototype for 'drawtrackbox' [-Wmissing-prototypes]`
  - ? 2 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `lib/reslook.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 4 warnings, 0 errors.
- **other compiler diagnostic:** compiler diagnostic; 4 distinct warning/error lines
  - `lib\reslook.c:7:6: warning: no previous prototype for 'locateshapes' [-Wmissing-prototypes]`
  - `lib\reslook.c:15:10: warning: no previous prototype for 'locateshapesz' [-Wmissing-prototypes]`
  - `lib\reslook.c:27:10: warning: no previous prototype for 'file_find_sounds_required' [-Wmissing-prototypes]`
  - ? 1 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `lib/audiores.c`

- Compile: syntax `failed`, object `failed`; distinct diagnostics: 67 warnings, 1 errors.
- BLOCKER CLASS: 2 (legacy type, declaration, or ABI assumptions fail under the flat host types).
- **implicit int widths:** compiler diagnostic; 31 distinct warning/error lines
  - `lib\audiores.c:115:5: warning: conflicting types for built-in function 'strlen'; expected 'long long unsigned int(const char *)' [-Wbuiltin-declaration-mismatch]`
  - `lib\audiores.c:130:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]`
  - `lib\audiores.c:183:6: warning: ISO C forbids conversion of object pointer to function pointer type [-Wpedantic]`
  - ? 28 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 2 distinct warning/error lines
  - `lib\audiores.c:708:28: warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]`
  - `lib\audiores.c:709:27: warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]`
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** explicit packing/alignment directive; aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 35 distinct warning/error lines
  - `lib\audiores.c:132:16: warning: no previous prototype for 'init_audio_resources' [-Wmissing-prototypes]`
  - `lib\audiores.c:172:10: warning: no previous prototype for 'load_audio_finalize' [-Wmissing-prototypes]`
  - `lib\audiores.c:197:10: warning: no previous prototype for 'audio_pause_playback' [-Wmissing-prototypes]`
  - ? 32 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `lib/audioply.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 51 warnings, 0 errors.
- **implicit int widths:** compiler diagnostic; 42 distinct warning/error lines
  - `lib\audioply.c:188:22: warning: ISO C forbids conversion of object pointer to function pointer type [-Wpedantic]`
  - `lib\audioply.c:216:52: warning: conversion to 'uint8_t' {aka 'unsigned char'} from 'char' may change the sign of the result [-Wsign-conversion]`
  - `lib\audioply.c:220:26: warning: ISO C forbids conversion of object pointer to function pointer type [-Wpedantic]`
  - ? 39 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **pointer size assumptions:** compiler diagnostic; 3 distinct warning/error lines
  - `lib\audioply.c:169:48: warning: pointer targets in passing argument 2 of 'read_audio_event' differ in signedness [-Wpointer-sign]`
  - `lib\audioply.c:197:34: warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]`
  - `lib\audioply.c:272:65: warning: pointer targets in passing argument 2 of 'read_audio_event' differ in signedness [-Wpointer-sign]`
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 6 distinct warning/error lines
  - `lib\audioply.c:113:18: warning: no previous prototype for 'audiodriver_timer' [-Wmissing-prototypes]`
  - `lib\audioply.c:366:18: warning: no previous prototype for 'start_audio_voice_sample' [-Wmissing-prototypes]`
  - `lib\audioply.c:489:18: warning: no previous prototype for 'send_audio_stop_event' [-Wmissing-prototypes]`
  - ? 3 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `lib/audiochk.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 8 warnings, 0 errors.
- **implicit int widths:** compiler diagnostic; 4 distinct warning/error lines
  - `lib\audiochk.c:3:16: warning: conflicting types for built-in function 'cap'; expected 'int(int)' [-Wbuiltin-declaration-mismatch]`
  - `lib\audiochk.c:39:31: warning: conversion to 'uint8_t' {aka 'unsigned char'} from 'char' may change the sign of the result [-Wsign-conversion]`
  - `lib\audiochk.c:59:46: warning: conversion to 'int16_t' {aka 'short int'} from 'uint16_t' {aka 'short unsigned int'} may change the sign of the result [-Wsign-conversion]`
  - ? 1 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **other compiler diagnostic:** compiler diagnostic; 4 distinct warning/error lines
  - `lib\audiochk.c:8:9: warning: no previous prototype for 'audioresource_compare_chunknames' [-Wmissing-prototypes]`
  - `lib\audiochk.c:29:9: warning: no previous prototype for 'audioresource_get_chunk_index' [-Wmissing-prototypes]`
  - `lib\audiochk.c:49:14: warning: no previous prototype for 'audioresource_find' [-Wmissing-prototypes]`
  - ? 1 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `lib/startup.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 21 warnings, 0 errors.
- **implicit int widths:** compiler diagnostic; 11 distinct warning/error lines
  - `lib\startup.c:88:17: warning: conflicting types for built-in function 'exit'; expected 'void(int)' [-Wbuiltin-declaration-mismatch]`
  - `lib\startup.c:108:16: warning: conflicting types for built-in function 'strlen'; expected 'long long unsigned int(const char *)' [-Wbuiltin-declaration-mismatch]`
  - `lib\startup.c:206:40: warning: array subscript has type 'char' [-Wchar-subscripts]`
  - ? 8 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 10 distinct warning/error lines
  - `lib\startup.c:16:11: warning: no previous prototype for 'file_load_shape2d_nofatal2' [-Wmissing-prototypes]`
  - `lib\startup.c:25:10: warning: no previous prototype for 'file_combine_and_find' [-Wmissing-prototypes]`
  - `lib\startup.c:39:11: warning: no previous prototype for 'myfindnext' [-Wmissing-prototypes]`
  - ? 7 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `lib/lineedit.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 5 warnings, 0 errors.
- **implicit int widths:** compiler diagnostic; 2 distinct warning/error lines
  - `lib\lineedit.c:16:12: warning: conflicting types for built-in function 'strlen'; expected 'long long unsigned int(const char *)' [-Wbuiltin-declaration-mismatch]`
  - `lib\lineedit.c:280:9: warning: conversion from 'int' to 'int16_t' {aka 'short int'} may change value [-Wconversion]`
- **segment arithmetic:** explicit segment/offset helper or spelling
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 3 distinct warning/error lines
  - `lib\lineedit.c:46:9: warning: no previous prototype for 'get_pstring' [-Wmissing-prototypes]`
  - `lib\lineedit.c:124:125: warning: suggest parentheses around '&&' within '||' [-Wparentheses]`
  - `lib\lineedit.c:124:170: warning: suggest parentheses around '&&' within '||' [-Wparentheses]`

### `lib/shp2res.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 11 warnings, 0 errors.
- **implicit int widths:** compiler diagnostic; 8 distinct warning/error lines
  - `lib\shp2res.c:62:21: warning: conversion to 'int16_t' {aka 'short int'} from 'uint16_t' {aka 'short unsigned int'} may change the sign of the result [-Wsign-conversion]`
  - `lib\shp2res.c:64:43: warning: conversion to 'uint16_t' {aka 'short unsigned int'} from 'int16_t' {aka 'short int'} may change the sign of the result [-Wsign-conversion]`
  - `lib\shp2res.c:98:13: warning: conversion to 'int16_t' {aka 'short int'} from 'uint16_t' {aka 'short unsigned int'} may change the sign of the result [-Wsign-conversion]`
  - ? 5 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 3 distinct warning/error lines
  - `lib\shp2res.c:31:11: warning: no previous prototype for 'loadandshapepack' [-Wmissing-prototypes]`
  - `lib\shp2res.c:39:11: warning: no previous prototype for 'loadandshapepackz' [-Wmissing-prototypes]`
  - `lib\shp2res.c:92:9: warning: unused variable 'baseOff' [-Wunused-variable]`

### `lib/polarrad.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\polarrad.c:7:5: warning: no previous prototype for 'int3dhypot' [-Wmissing-prototypes]`

### `lib/sphhelp.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\sphhelp.c:11:6: warning: no previous prototype for 'solidellipseh' [-Wmissing-prototypes]`

### `lib/sphhelp2.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\sphhelp2.c:10:10: warning: no previous prototype for 'ellipseh' [-Wmissing-prototypes]`

### `lib/wheel.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\wheel.c:18:10: warning: no previous prototype for 'solidtire' [-Wmissing-prototypes]`

### `lib/whlhelp.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\whlhelp.c:11:10: warning: no previous prototype for 'tire' [-Wmissing-prototypes]`

### `lib/whlhelp2.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\whlhelp2.c:11:6: warning: no previous prototype for 'ring' [-Wmissing-prototypes]`

### `lib/whlhelp3.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 1 warnings, 0 errors.
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\whlhelp3.c:12:6: warning: no previous prototype for 'ellipse' [-Wmissing-prototypes]`

### `lib/mouse.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 13 warnings, 0 errors.
- **implicit int widths:** bare int/unsigned int spelling (host int is 32-bit; DOS MSC int is 16-bit); 5 distinct warning/error lines
  - `lib\mouse.c:60:36: warning: overflow in conversion from 'int' to 'int16_t' {aka 'short int'} changes value from '65535' to '-1' [-Woverflow]`
  - `lib\mouse.c:62:12: warning: conversion to 'int16_t' {aka 'short int'} from 'uint16_t' {aka 'short unsigned int'} may change the sign of the result [-Wsign-conversion]`
  - `lib\mouse.c:96:12: warning: conversion from 'unsigned int' to 'uint16_t' {aka 'short unsigned int'} may change value [-Wconversion]`
  - ? 2 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.
- **int86/port I/O:** DOS interrupt, port-I/O intrinsic, or intrinsic pragma
- **signed shift:** shift expression; inspect signed operands and overflow assumptions
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 8 distinct warning/error lines
  - `lib\mouse.c:44:9: warning: no previous prototype for 'initmouse' [-Wmissing-prototypes]`
  - `lib\mouse.c:130:10: warning: no previous prototype for 'setmousepos' [-Wmissing-prototypes]`
  - `lib\mouse.c:145:10: warning: no previous prototype for 'getmousepos' [-Wmissing-prototypes]`
  - ? 5 additional categorized diagnostic rows; see `build/porting/host-probe/host/results.json`.

### `lib/matrot.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 3 warnings, 0 errors.
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 3 distinct warning/error lines
  - `lib\matrot.c:12:6: warning: no previous prototype for 'xformx' [-Wmissing-prototypes]`
  - `lib\matrot.c:24:6: warning: no previous prototype for 'xformy' [-Wmissing-prototypes]`
  - `lib\matrot.c:36:6: warning: no previous prototype for 'xformz' [-Wmissing-prototypes]`

### `lib/mcgawnd.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 2 warnings, 0 errors.
- **far pointer normalization:** segmented pointer qualifier is erased to a flat host pointer
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries
- **other compiler diagnostic:** compiler diagnostic; 2 distinct warning/error lines
  - `lib\mcgawnd.c:23:10: warning: no previous prototype for 'setup_mcgawnd1' [-Wmissing-prototypes]`
  - `lib\mcgawnd.c:38:10: warning: no previous prototype for 'setup_mcgawnd2' [-Wmissing-prototypes]`

### `lib/shp2file.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 2 warnings, 0 errors.
- **other compiler diagnostic:** compiler diagnostic; 2 distinct warning/error lines
  - `lib\shp2file.c:55:11: warning: no previous prototype for 'loadshape' [-Wmissing-prototypes]`
  - `lib\shp2file.c:64:11: warning: no previous prototype for 'loadshapez' [-Wmissing-prototypes]`

### `lib/sprrect.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 2 warnings, 0 errors.
- **implicit int widths:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\sprrect.c:12:13: warning: conversion from 'int' to 'int16_t' {aka 'short int'} may change value [-Wconversion]`
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\sprrect.c:8:10: warning: no previous prototype for 'sprite_draw_rect_outline' [-Wmissing-prototypes]`

### `lib/rectout.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 3 warnings, 0 errors.
- **implicit int widths:** compiler diagnostic; 2 distinct warning/error lines
  - `lib\rectout.c:14:13: warning: conversion from 'int' to 'int16_t' {aka 'short int'} may change value [-Wconversion]`
  - `lib\rectout.c:15:14: warning: conversion from 'int' to 'int16_t' {aka 'short int'} may change value [-Wconversion]`
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\rectout.c:9:6: warning: no previous prototype for 'xorbox' [-Wmissing-prototypes]`

### `lib/toupper.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 2 warnings, 0 errors.
- **implicit int widths:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\toupper.c:5:5: warning: conflicting types for built-in function 'cap'; expected 'int(int)' [-Wbuiltin-declaration-mismatch]`
- **other compiler diagnostic:** compiler diagnostic; 1 distinct warning/error lines
  - `lib\toupper.c:5:5: warning: no previous prototype for 'cap' [-Wmissing-prototypes]`

### `game/trkconst.c`

- Compile: syntax `ok`, object `ok`; distinct diagnostics: 0 warnings, 0 errors.
- **struct packing:** aggregate layout needs host review where it crosses file/API or serialized-data boundaries

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
