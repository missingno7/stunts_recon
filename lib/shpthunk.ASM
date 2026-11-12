; ASMDOC Module: Seven far tail-forwarders into the C 2D shape loader and run encoder.
; ASMDOC Owner file_load_shape2d_thunks; original source asm/file_load_shape2d_thunks.ASM.
; ASMDOC Routine inventory: _file_load_shape2d_res_fatal_thunk, _load_shape2d_res_nofatal_thunk,
; _file_load_shape2d_res_thunk,
; ASMDOC _parse_shape2d_thunk, _file_load_shape2d_fatal_thunk, _load_shape2d_nofatal_thunk, _file_load_shape2d_thunk
; ASMDOC All BP stack offsets below follow PUSH BP / MOV BP,SP; data spans are caller-owned.
; ASMDOC Seven code publics; first source public is _file_load_shape2d_res_fatal_thunk, while MASM OMF recipe
; public is truncated to
; ASMDOC _file_load_shape2d_res_fatal_th. They are one entry, not two.
; ASMDOC No self-modification, no local globals and no local interrupt; far forwarding does not make callee
; effects empty.
.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
S012_TEXT ends
assume cs:S012_TEXT
extrn _loadandshapepack:far
extrn _loadandshapepackz:far
extrn _loadandshapepacka:far
extrn _shapepack:far
extrn _loadshape:far
extrn _loadshapez:far
extrn _file_load_shape2d:far
public _file_load_shape2d_res_fatal_thunk
public _load_shape2d_res_nofatal_thunk
public _file_load_shape2d_res_thunk
public _parse_shape2d_thunk
public _file_load_shape2d_fatal_thunk
public _load_shape2d_nofatal_thunk
public _file_load_shape2d_thunk
S012_TEXT segment word public 'CODE'

; -----------------------------------------------------------------------------
; Purpose: Call the shape resource loader with fatal-error handling enabled.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_load_shape2d_res_fatal_thunk
; ASMDOC purpose: Tail-forward unchanged caller frame to _file_load_shape2d_res_fatal to load/transform a
; managed 2D shape archive.
; ASMDOC convention: FAR tail JMP, original caller-clean frame forwarded byte-for-byte; target stack offsets are
; after its PUSH BP / MOV
; ASMDOC BP,SP.
; ASMDOC inputs: BP+6 near resource name; fatal policy fixed to 1 in target; Offsets stated for the target BP
; frame; thunk does not push BP
; ASMDOC or adjust SP; near names use DGROUP DS
; ASMDOC outputs: No local return: target RETF returns to original caller; No defined value for parse target;
; otherwise target DX:AX managed
; ASMDOC far archive or null on recoverable failure
; ASMDOC preserves: All registers/flags unchanged by the jump itself; target MSC routine restores BP SI DI DS
; according to its implementation
; ASMDOC clobbers: Target AX BX CX DX ES flags and transitive service effects; thunk adds no clobber
; ASMDOC globals_read: No direct data reads; target can read shape_extensions, shape2d_extension_suffixes,
; palmap and memory-manager state
; ASMDOC globals_written: No direct writes; target writes managed allocations and memory-manager/cache state;
; shape loader may initialize
; ASMDOC shared palmap
; ASMDOC services: Direct FAR JMP _file_load_shape2d_res_fatal; no additional return address; effects are
; exactly the named target and its
; ASMDOC callees; Targets in lib/shp2res.c and lib/shp2file.c call memory allocation/cache,
; filesystem/decompression and
; ASMDOC shape codecs as needed; no service is executed locally
; ASMDOC dead: True
; ASMDOC Public name from existing plan: _file_load_shape2d_res_fatal_thunk (unchanged).
_file_load_shape2d_res_fatal_thunk proc far

; ASMDOC Tail jump preserves the existing far return address; all target side effects remain part of this ABI.
    jmp     _loadandshapepack
_file_load_shape2d_res_fatal_thunk endp
; -----------------------------------------------------------------------------
; Purpose: Call the shape resource loader with fatal-error handling disabled.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _load_shape2d_res_nofatal_thunk
; ASMDOC purpose: Tail-forward unchanged caller frame to _file_load_shape2d_res_nofatal to load/transform a
; managed 2D shape archive.
; ASMDOC convention: FAR tail JMP, original caller-clean frame forwarded byte-for-byte; target stack offsets are
; after its PUSH BP / MOV
; ASMDOC BP,SP.
; ASMDOC inputs: BP+6 near resource name; fatal policy fixed to 0 in target; Offsets stated for the target BP
; frame; thunk does not push BP
; ASMDOC or adjust SP; near names use DGROUP DS
; ASMDOC outputs: No local return: target RETF returns to original caller; No defined value for parse target;
; otherwise target DX:AX managed
; ASMDOC far archive or null on recoverable failure
; ASMDOC preserves: All registers/flags unchanged by the jump itself; target MSC routine restores BP SI DI DS
; according to its implementation
; ASMDOC clobbers: Target AX BX CX DX ES flags and transitive service effects; thunk adds no clobber
; ASMDOC globals_read: No direct data reads; target can read shape_extensions, shape2d_extension_suffixes,
; palmap and memory-manager state
; ASMDOC globals_written: No direct writes; target writes managed allocations and memory-manager/cache state;
; shape loader may initialize
; ASMDOC shared palmap
; ASMDOC services: Direct FAR JMP _file_load_shape2d_res_nofatal; no additional return address; effects are
; exactly the named target and its
; ASMDOC callees; Targets in lib/shp2res.c and lib/shp2file.c call memory allocation/cache,
; filesystem/decompression and
; ASMDOC shape codecs as needed; no service is executed locally
; ASMDOC dead: False
; ASMDOC Public name from existing plan: _load_shape2d_res_nofatal_thunk (unchanged).
_load_shape2d_res_nofatal_thunk proc far

    jmp     _loadandshapepackz
_load_shape2d_res_nofatal_thunk endp
; -----------------------------------------------------------------------------
; Purpose: Forward to the shared shape resource loader.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_load_shape2d_res_thunk
; ASMDOC purpose: Tail-forward unchanged caller frame to _file_load_shape2d_res to load/transform a managed 2D
; shape archive.
; ASMDOC convention: FAR tail JMP, original caller-clean frame forwarded byte-for-byte; target stack offsets are
; after its PUSH BP / MOV
; ASMDOC BP,SP.
; ASMDOC inputs: BP+6 near resource name; BP+8 fatal policy; Offsets stated for the target BP frame; thunk does
; not push BP or adjust SP;
; ASMDOC near names use DGROUP DS
; ASMDOC outputs: No local return: target RETF returns to original caller; No defined value for parse target;
; otherwise target DX:AX managed
; ASMDOC far archive or null on recoverable failure
; ASMDOC preserves: All registers/flags unchanged by the jump itself; target MSC routine restores BP SI DI DS
; according to its implementation
; ASMDOC clobbers: Target AX BX CX DX ES flags and transitive service effects; thunk adds no clobber
; ASMDOC globals_read: No direct data reads; target can read shape_extensions, shape2d_extension_suffixes,
; palmap and memory-manager state
; ASMDOC globals_written: No direct writes; target writes managed allocations and memory-manager/cache state;
; shape loader may initialize
; ASMDOC shared palmap
; ASMDOC services: Direct FAR JMP _file_load_shape2d_res; no additional return address; effects are exactly the
; named target and its callees;
; ASMDOC Targets in lib/shp2res.c and lib/shp2file.c call memory allocation/cache,
; filesystem/decompression and shape
; ASMDOC codecs as needed; no service is executed locally
; ASMDOC dead: True
; ASMDOC Public name from existing plan: _file_load_shape2d_res_thunk (unchanged).
_file_load_shape2d_res_thunk proc far

    jmp     _loadandshapepacka
_file_load_shape2d_res_thunk endp
; -----------------------------------------------------------------------------
; Purpose: Forward to the shared 2D shape parser.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _parse_shape2d_thunk
; ASMDOC purpose: Tail-forward unchanged caller frame to _parse_shape2d to encode shape pixels as literal/repeat
; runs and resize destination.
; ASMDOC convention: FAR tail JMP, original caller-clean frame forwarded byte-for-byte; target stack offsets are
; after its PUSH BP / MOV
; ASMDOC BP,SP.
; ASMDOC inputs: BP+6 source archive offset; +8 segment; BP+10 destination archive offset; +12 segment; Offsets
; stated for the target BP
; ASMDOC frame; thunk does not push BP or adjust SP; near names use DGROUP DS
; ASMDOC outputs: No local return: target RETF returns to original caller; No defined value for parse target;
; otherwise target DX:AX managed
; ASMDOC far archive or null on recoverable failure
; ASMDOC preserves: All registers/flags unchanged by the jump itself; target MSC routine restores BP SI DI DS
; according to its implementation
; ASMDOC clobbers: Target AX BX CX DX ES flags and transitive service effects; thunk adds no clobber
; ASMDOC globals_read: No direct data reads; target reads source archive and headers
; ASMDOC globals_written: No direct writes; target writes destination archive and resizes its allocation
; ASMDOC services: Direct FAR JMP _parse_shape2d; no additional return address; effects are exactly the named
; target and its callees; Targets
; ASMDOC in lib/shp2res.c and lib/shp2file.c call memory allocation/cache,
; filesystem/decompression and shape codecs as
; ASMDOC needed; no service is executed locally
; ASMDOC dead: True
; ASMDOC Public name from existing plan: _parse_shape2d_thunk (unchanged).
_parse_shape2d_thunk proc far

    jmp     _shapepack
_parse_shape2d_thunk endp
; -----------------------------------------------------------------------------
; Purpose: Call the shape loader with fatal-error handling enabled.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_load_shape2d_fatal_thunk
; ASMDOC purpose: Tail-forward unchanged caller frame to _file_load_shape2d_fatal to load/transform a managed 2D
; shape archive.
; ASMDOC convention: FAR tail JMP, original caller-clean frame forwarded byte-for-byte; target stack offsets are
; after its PUSH BP / MOV
; ASMDOC BP,SP.
; ASMDOC inputs: BP+6 near filename; fatal policy fixed to 1 in target; Offsets stated for the target BP frame;
; thunk does not push BP or
; ASMDOC adjust SP; near names use DGROUP DS
; ASMDOC outputs: No local return: target RETF returns to original caller; No defined value for parse target;
; otherwise target DX:AX managed
; ASMDOC far archive or null on recoverable failure
; ASMDOC preserves: All registers/flags unchanged by the jump itself; target MSC routine restores BP SI DI DS
; according to its implementation
; ASMDOC clobbers: Target AX BX CX DX ES flags and transitive service effects; thunk adds no clobber
; ASMDOC globals_read: No direct data reads; target can read shape_extensions, shape2d_extension_suffixes,
; palmap and memory-manager state
; ASMDOC globals_written: No direct writes; target writes managed allocations and memory-manager/cache state;
; shape loader may initialize
; ASMDOC shared palmap
; ASMDOC services: Direct FAR JMP _file_load_shape2d_fatal; no additional return address; effects are exactly
; the named target and its
; ASMDOC callees; Targets in lib/shp2res.c and lib/shp2file.c call memory allocation/cache,
; filesystem/decompression and
; ASMDOC shape codecs as needed; no service is executed locally
; ASMDOC dead: False
; ASMDOC Public name from existing plan: _file_load_shape2d_fatal_thunk (unchanged).
_file_load_shape2d_fatal_thunk proc far

    jmp     _loadshape
_file_load_shape2d_fatal_thunk endp
; -----------------------------------------------------------------------------
; Purpose: Call the shape loader with fatal-error handling disabled.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _load_shape2d_nofatal_thunk
; ASMDOC purpose: Tail-forward unchanged caller frame to _file_load_shape2d_nofatal to load/transform a managed
; 2D shape archive.
; ASMDOC convention: FAR tail JMP, original caller-clean frame forwarded byte-for-byte; target stack offsets are
; after its PUSH BP / MOV
; ASMDOC BP,SP.
; ASMDOC inputs: BP+6 near filename; fatal policy fixed to 0 in target; Offsets stated for the target BP frame;
; thunk does not push BP or
; ASMDOC adjust SP; near names use DGROUP DS
; ASMDOC outputs: No local return: target RETF returns to original caller; No defined value for parse target;
; otherwise target DX:AX managed
; ASMDOC far archive or null on recoverable failure
; ASMDOC preserves: All registers/flags unchanged by the jump itself; target MSC routine restores BP SI DI DS
; according to its implementation
; ASMDOC clobbers: Target AX BX CX DX ES flags and transitive service effects; thunk adds no clobber
; ASMDOC globals_read: No direct data reads; target can read shape_extensions, shape2d_extension_suffixes,
; palmap and memory-manager state
; ASMDOC globals_written: No direct writes; target writes managed allocations and memory-manager/cache state;
; shape loader may initialize
; ASMDOC shared palmap
; ASMDOC services: Direct FAR JMP _file_load_shape2d_nofatal; no additional return address; effects are exactly
; the named target and its
; ASMDOC callees; Targets in lib/shp2res.c and lib/shp2file.c call memory allocation/cache,
; filesystem/decompression and
; ASMDOC shape codecs as needed; no service is executed locally
; ASMDOC dead: False
; ASMDOC Public name from existing plan: _load_shape2d_nofatal_thunk (unchanged).
_load_shape2d_nofatal_thunk proc far

    jmp     _loadshapez
_load_shape2d_nofatal_thunk endp
; -----------------------------------------------------------------------------
; Purpose: Forward to the shared 2D shape loader.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_load_shape2d_thunk
; ASMDOC purpose: Tail-forward unchanged caller frame to _file_load_shape2d to load/transform a managed 2D shape
; archive.
; ASMDOC convention: FAR tail JMP, original caller-clean frame forwarded byte-for-byte; target stack offsets are
; after its PUSH BP / MOV
; ASMDOC BP,SP.
; ASMDOC inputs: BP+6 near filename; BP+8 fatal policy; Offsets stated for the target BP frame; thunk does not
; push BP or adjust SP; near
; ASMDOC names use DGROUP DS
; ASMDOC outputs: No local return: target RETF returns to original caller; No defined value for parse target;
; otherwise target DX:AX managed
; ASMDOC far archive or null on recoverable failure
; ASMDOC preserves: All registers/flags unchanged by the jump itself; target MSC routine restores BP SI DI DS
; according to its implementation
; ASMDOC clobbers: Target AX BX CX DX ES flags and transitive service effects; thunk adds no clobber
; ASMDOC globals_read: No direct data reads; target can read shape_extensions, shape2d_extension_suffixes,
; palmap and memory-manager state
; ASMDOC globals_written: No direct writes; target writes managed allocations and memory-manager/cache state;
; shape loader may initialize
; ASMDOC shared palmap
; ASMDOC services: Direct FAR JMP _file_load_shape2d; no additional return address; effects are exactly the
; named target and its callees;
; ASMDOC Targets in lib/shp2res.c and lib/shp2file.c call memory allocation/cache,
; filesystem/decompression and shape
; ASMDOC codecs as needed; no service is executed locally
; ASMDOC dead: True
; ASMDOC Public name from existing plan: _file_load_shape2d_thunk (unchanged).
_file_load_shape2d_thunk proc far

; ASMDOC Tail jump followed by an unreachable alignment byte, retained as owned extent.
    jmp     _file_load_shape2d
    db 0 ; retained zero fill at this boundary
_file_load_shape2d_thunk endp

S012_TEXT ends
end
