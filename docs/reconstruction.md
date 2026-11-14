# The reconstruction: what is proven, what is our choice, what is unknown

This is the closing statement of the source-side work on Stunts 1.1 (MCGA). It separates three things
that are easy to blur: facts the byte-exact proof establishes about the original program, decisions we
made to turn matching code into readable source, and questions that stay open.

## What the tree is

`game/`, `lib/` and `include/` are source code for the original executable: 38 C files and 40 MASM
modules. Built with the pinned historical toolchain they reproduce the original load image, all 2,588
relocations in their original order, and the packed executable.

| Part | Bytes | Built with |
|---|---:|---|
| C code and data | 154,618 | Microsoft C 5.10, medium model, `/AM /O /Gs`; `game/audioeng.c` alone with Microsoft C 6.00, `/AM /Os /Oe /Og /Gs /Zi` |
| Assembly code and data | 36,552 | MASM 5.10, `/Mx` |
| Microsoft C runtime | 8,768 | pinned library members, selected by the linker |
| Raw bytes copied from the original | 0 | |

The link is an ordinary library-search link with LINK 3.65 followed by EXEPACK.

- `python tools/validate.py` is the gate: it proves the image, the relocations and every object.
- `python tools/histbuild.py` rebuilds the executable from sources alone and reports `BUILT_EXACT`.
- [acceptance.md](acceptance.md) states the rules the proof applies; [msc510-codegen.md](msc510-codegen.md)
  collects the compiler behaviour learned on the way.

Every change to the sources is proven by the gate, never assumed neutral. The reasons are in the next
section: in this toolchain, names and the order of declarations reach the bytes.

## Proven about the original

**Toolchain.** The code is reproduced by Microsoft C 5.10 with the flags above. One file, the audio
engine, is reproduced by Microsoft C 6.00 with debug information enabled. The accepted functions do
not distinguish 5.10 from 5.00, so 5.10 is the profile that works, not a proven version.

**Game and library.** Ten code objects are linked before the C runtime and everything else after it. That is
the layout of a program linked as a handful of game objects plus a support library, and it is the only
evidence behind the `game/` and `lib/` split: `game/` holds the ten objects linked first.

**Original names.** The FM Towns release of the same source kept a symbol table. Matching its functions
and data against this executable gave 448 original names (359 functions, 89 data). About 357 of them are
in the sources now.

**The main file defines the shared simulation state.** MSC's optimiser sizes an internal budget from the
symbols a file defines, including the length of their names. In `game/frontend.c` one function
(`end_hiscore`) sits on a threshold of that budget: with the original, shorter function names the code
only comes out right if the file defines more globals than it uses. Adding tentative definitions of 57
globals of the simulation to the main file, together with the original names, makes all 78 objects and
the link exact. The effect is specific to this file; the other files tolerate little or no extra
material of this kind. So the original main file defined a block of game state that other files only declared. The exact membership of
the block is not unique: any prefix of 46 to 57 of the globals in our order works.

**Declarations inside functions.** In `game/uires.c` and `game/render.c` several functions need their
own `extern` declaration for a global; a file-level declaration changes the object. The original source
had `extern` lines inside those functions.

**Declaration order is observable.** MSC 5.10 emits external and communal names in an order derived from
its internal symbol numbers, and LINK 3.65 places uninitialised globals by a hash of the name in the
order it first sees them. The 303 uninitialised globals therefore constrain which file declares what
first. This is why moving a declaration between headers can move a global in memory.

**One declaration per entity, mostly.** Our files originally disagreed on the type of 235 functions,
globals and records. Compiling every candidate in every file showed that a single declaration is exact
everywhere for 192 of them; later work resolved more. Most disagreements were artefacts of matching each
file separately, not properties of the original.

## Our choices

None of the following is evidence about the original. Each was chosen to make the tree readable and can
be changed freely as long as the gate passes.

- **File names.** `frontend.c`, `carsim.c`, `gfxres.ASM` and the rest are ours, kept to DOS 8.3 form. The
  original file names are unknown.
- **Headers.** The division into 44 header files by subsystem is ours. The original headers are unknown;
  only the constraints above (what must be declared before what, and where) come from the bytes.
- **Names without an FM Towns counterpart**, all record member names, and all named constants. The
  values of the constants are proven; their names are descriptions.
- **Comments.**
- **The file boundaries inside `lib/`.** Each object is a unit the linker placed as a whole, so the
  objects are real; which source lines belonged to which original file within one object is not known.
- **The order of the 57 definitions in `game/frontend.c`** was fitted so that every global lands at its
  original address.

## Unknown or left open

- **Six records still have per-file definitions:** `GAMESTATE`, `SIMD`, `SPRITE`, `SHAPE3D`, `SHAPE2D`
  and `TRKOBJINFO`. For each, one merged definition exists that compiles to identical code in every
  file. Adopting them changes the order in which globals are first declared and moves some of them; the
  declaration order that fixes this was not found. This is an ordering puzzle, not a doubt about the
  records.
- **83 per-file declarations remain in total,** the views of those six records among them, each with
  a one-line reason (`Local view:` in the sources). Several are blocked by the same ordering effect.
- **About 90 of the 448 original names are not applied.** The proof rejected 74 names when tried (15
  functions, 42 globals, 17 record members): the length of the name or its position changes an object
  or the placement of a global. Where that happened the earlier reconstruction name is still in place.
- **Compiler version** (5.00 or 5.10) for all files except the audio engine, and why the audio engine
  was built with a different compiler and with debug information.
- **The original build procedure**: makefile, compiler invocation per file, and the source file
  boundaries beyond what the link order shows. Our per-file build parameters are in `recipes/`.
- **Which assembly modules were written by hand.** The 40 MASM modules reproduce their bytes as
  assembly; whether any of them was compiled from C is not established.
- **`goto` statements.** The C sources contain 158. Whether each was in the original or is our way to
  obtain the original block order has not been established.
- **Three routines in `game/rpmrand.ASM`** issue INT 60h to 62h without setting registers; no caller or
  provider is known.

Questions that concern the running game and its data formats, not the source, are listed in
[unresolved.md](unresolved.md).

## Working on the sources

`README.md` lists the commands. The short form: try a change in a scratch directory with
`tools/promote.py` or `tools/rename.py`, which prove the affected objects and the real link before they
write anything, and run `python tools/validate.py` at the end. Renames can fail for the reasons above;
that is the proof working, not a tool defect.
