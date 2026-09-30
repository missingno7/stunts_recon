# Routine differential harness

This package calls the locked, unpacked MCGA DOS image in Unicorn x86 16-bit
real mode, then calls a `PORT_BUILD` C routine from an i686 MinGW DLL. It
compares the named memory regions, declared return registers, and optional
flags. The MZ image is reconstructed and checked against
`layout/oracle.lock.json` in memory; harness runs do not write or regenerate
the oracle.

## Run the examples

Requirements: Python 3.10+, `unicorn==2.1.4`, and the i686 MinGW GCC used by
the port (default `C:\msys64\mingw32\bin\gcc.exe`). Pass the immutable asset
directory explicitly if this worktree has no local `assets/` directory:

```powershell
python -m tools.porting.diffharness --assets D:\Prog\stunts_recon\assets
```

The host test DLL and its intermediate build output go under ignored
`build/porting/diffharness/`. A 64-bit Python cannot load an i686 DLL. In that
case the package uses `C:\msys64\mingw32\bin\python.exe` as a small 32-bit
`ctypes` worker; set `DIFFHARNESS_PYTHON32` if it lives elsewhere. When Python
is already 32-bit, the DLL loads directly through `ctypes`.

The three passing calls cover `sprite_1_unk3`, `draw_filled_rect`, and
`mulscl`. Both renderer cases compare all 65,536 bytes at A000:0000 after
starting with the same framebuffer contents. The negative example flips one
byte of the port result and confirms the comparator reports the first byte and
the differing-byte count.

## Add a routine case

`RoutineCase` is the call contract. `routine` accepts a reviewed name, stable
function ID, or load-image address. Stack words are listed in source argument
order (argument zero first); `call="far"` builds the inter-segment return
frame and `call="near"` builds a near return frame. Set `result_registers`
only for registers in the original routine's return contract. `MemoryWrite`
installs bytes at a DOS segment:offset after loading and applying every MZ
relocation. `MemoryRegion` snapshots the same address from both machines.

For example, this adds a `mulscl` vector in fewer than twenty lines:

```python
from tools.porting.diffharness import CallResult, DiffHarness, RoutineCase
from pathlib import Path

left, right = -12345, 4567
case = RoutineCase(
    "mulscl vector", "mulscl", args=[left & 0xffff, right], call="far",
    result_registers=("ax",),
    port_call=lambda port, _: CallResult(
        {"ax": port.call_mulscl(left, right)}, 0, {}, 0, "cdecl"))
DiffHarness(asset_dir=Path(r"D:\Prog\stunts_recon\assets")).run(case)
```

The generic machine side loads the 200,000-byte image at segment `1000h`,
applies the locked relocation table, resolves names through
`layout/code-symbols.json` and `layout/names-registry.json`, and selects the
reviewed code-segment frame for the entry point. DS starts at the original
`3B87h` DGROUP anchor, ES defaults to `A000h`, and SS:SP use an isolated DOS
stack. Override any entry register with `registers={"ds": ..., "es": ...}`.
Unspecified DOS RAM starts zeroed; add synthetic state or asset-derived state
with `memory=[MemoryWrite(...)]`. RET and RETF stop at a sentinel, and the
instruction budget defaults to one million instructions.

INT instructions and unconfigured IN/OUT accesses stop the call and raise a
`TrapError` with the vector or port and current CS:IP. `RealModeRunner` accepts
an `io_ports={port: value}` map for simple IN reads. A `RoutineCase` can set
`compare_flags=True` when flags are part of its contract; port adapters return
their declared registers, flags, and region bytes in a `CallResult`.

## PORT_BUILD adapter

`port.py` builds the selected production files `port/sprite.c`,
`port/memory.c`, and `port/sincos.c` with the i686 compiler. The small checked-
in adapter exports only fixture setup, routine calls, and framebuffer reads;
it does not replace the production C routine bodies. Add another function to
`host_adapter.c`, bind it in `port.py`, and provide a `port_call` on its case.
For routines requiring globals or far buffers, give the C adapter an explicit
setup/copy boundary and describe the equivalent original memory with
`MemoryWrite`.
