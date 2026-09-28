# MSC 5.10 code-generation knowledge base

> **Guidance only.** These observations help choose *which source to try next*. They never make the
> verifier forgive a mismatch, never justify masking, trimming or normalising bytes, and never replace
> strict acceptance ([docs/acceptance.md](acceptance.md)). Toolchain *status* (canonical profile,
> per-object flags, symptoms S1-S8) lives only in
> [evidence/toolchain-hypotheses.json](../evidence/toolchain-hypotheses.json); entries below cite it by id.
> A rule here that disagrees with a strict compile of your own source is wrong for that context: trust the compiler.

Status tags: **VERIFIED** = reproduced for this document with the pinned compiler (listing under
`build/workers/kbdoc/out/`); **REPORTED** = taken from worker reports/supervisor notes, not reproduced here.
Unless stated otherwise the profile is pinned MSC 5.10 `CL /c /AM /O /Gs` (`msc510-medium`, register
`CC-MSC510-AM-O-Gs`). Addresses are image load offsets from `evidence/functions.json`.

## Contents
0. [Reproducing](#0-reproducing)
1. [Frame and locals](#1-frame-and-locals) L1-L11
2. [Expressions and registers](#2-expressions-and-registers) E1-E17
3. [Control-flow layout](#3-control-flow-layout) C1-C13
4. [Declarations, keywords, pragmas](#4-declarations-keywords-pragmas) D1-D8
5. [Object and OMF emission](#5-object-and-omf-emission) O1-O15
6. [Optimisation-flag signatures](#6-optimisation-flag-signatures) F1-F6
7. [Open, unexplained residuals](#7-open-unexplained-residuals) U1-U8
8. [Contradictions and corrections found while writing this](#8-contradictions-and-corrections)

## 0. Reproducing

```sh
# from the repository root; build/ is ignored, so reproducers are local scratch
sh build/workers/kbdoc/run_all.sh                       # regenerates every listing in build/workers/kbdoc/out/
python build/workers/kbdoc/cc.py build/workers/kbdoc/repro/r01_slot_hash.c [--flags "/AM /Oa /Gs"] \
       [--profile msc500-medium] [--records] [--raw]
```

- `cc.py` calls `tools/compiler.compile_source` (hash-verified CL.EXE under MS-DOS Player, the same path as
  `tools/search.py`) and prints each public with capstone, annotating FIXUPP fields. `--records` lists CODE
  LEDATA records `[seg, offset, size, nfixups]` with FIXUPP locations in record order and SEGDEF lengths.
  `--raw` bypasses the project preprocessor allow-list (diagnostic only; such sources cannot be promoted).
- Git Bash rewrites `/AM` into `C:/Program Files/Git/AM`; set `MSYS_NO_PATHCONV=1` (run_all.sh does).
- Capstone prints `98` as `cwde` (= CBW) and `99` as `cdq` (= CWD); `lcall 0,0` is `9A` far CALL.
- For a real target use `tools/search.py`, `tools/tubench.py`, `tools/slotorder.py`, `tools/cut_simulator.py`,
  `build/workers/permuter/blockdiff.py` and `build/workers/localsolver/` rather than rules of thumb.

## 1. Frame and locals

**L1 BP homes follow the identifier hash walk — VERIFIED** (r01; register `FACT-local-slot-order-identifier`).
`int c, b, a;` and `int a, b, c;` both give `a`=[bp-2], `b`=[bp-4], `c`=[bp-6]. Same bucket `a`/`q`
(97, 113 → 1): the later declaration gets [bp-2]. Rule: bucket = (sum of first 31 name bytes) & 15, buckets
ascending, newest declaration first inside a bucket, each home = even-rounded size cumulated from BP.
Corroboration: 19/19 accepted-source locals and 360 synthetic probes (hashrule); detect_penalty 0x7816
17/17 homes, build_track_object 0xE1A0 25/25 (localsolver, lsapply). Limits: temporaries, spills and
parameters are outside the rule. Tool: `tools/slotorder.py` (`--names` suggests names). Natural names that
satisfy the bucket are legitimate; meaningless hash names are not (preamble LOCAL NAMES).

**L2 Nested scopes reuse freed homes (LIFO, size-fit) — VERIFIED** (r02). Outer `a`=[bp-2]; `{long big;}`
takes [bp-6]; a sibling `{int small;}` and a later `{int x;}` reuse [bp-6]; frame 6. With the 2-byte block
first, a later `long` cannot reuse it and gets [bp-8] (frame 8). No splitting or coalescing. Release order of
containing scopes (direct homes before descendants) is REPORTED (hashrule, `nested_release_*.c`).

**L3 Explicit `register` locals: SI then DI in declaration order; homes still allocated — VERIFIED** (r03).
`register int y; register int a; int t;` → y=SI, a=DI; swapping declarations swaps SI/DI but not homes:
a=[bp-2] (bucket 1), t=[bp-4], y=[bp-6] (bucket 9) — the register homes are never referenced (frame 6).
Prologue `push bp; mov bp,sp; sub sp,N; push di; push si`, epilogue `pop si; pop di; mov sp,bp; pop bp; retf`.
REPORTED details (regrule): register parameters come first; only one-word ints/near pointers are eligible
(char, long, far pointers stay in memory); an initialised-but-dead candidate still consumes SI, an
uninitialised unreferenced one does not; SI/DI can also be used as compiler scratch (E1), so the save set is
not determined by declarations alone. Corroboration: copy_string, rect_adjust_from_point 0x1637A,
init_main 0x29E56, random_wait 0x2A25C, init_carstate_from_simd 0x6898 (all accepted).
Unreferenced 2-byte holes in a target frame are usually register-local homes (track_setup 0x106D4 holes
-946/-1852 = SI/DI indices; do_fileselect_dialog 0x17ED4; REPORTED localsolver/lsapply).

**L4 Under /O plain loop counters stay in memory; `register long` spills — VERIFIED** (r03).
`for (i=0;i<n;i++)` with plain `int i` → `inc word [bp-2]`; `register long l` keeps its [bp-4] home.
An enregistered counter with no home is an /Ol-family signature (F2), not a /O source shape
(`FLAG-Ol-plain-counter` FALSIFIED globally).

**L5 Compiler temporaries sit below all named locals — VERIFIED** (r05). With SI/DI busy holding call
results, `a = f(1)+g(2)+h(3)+k(4)` spills g()'s result to [bp-6] under named a=[bp-2], b=[bp-4].
REPORTED: draw_button 0x192DC has 8 temporaries at -100..-114; track_setup's [bp-2800] is a far-pointer
temporary; init_carstate_from_simd keeps `2*i`/`6*i` in two extra homes (hashrule).

**L6 A declared but unused local keeps its frame slot — VERIFIED** (r04). Adding `long unused_slot;`
(bucket 5) grows `sub sp,2` to `sub sp,6` and moves `x` (bucket 8) from [bp-2] to [bp-6]. Corroboration:
parse_shape2d 0x2AD9C needs `long baseOff` for its untouched [bp-22h] slot (miscc). Ruling in NOTES:
admissible only when the target frame has a never-referenced slot of exactly that size/alignment not
explained by a register home, temporary or struct member.

**L7 Frame holes can be unused struct members or array tails — REPORTED** (lsapply): build_track_object's
holes -0x30/-4 are unused `.y` of `struct VECTOR` locals; do_fileselect_dialog's 28-byte hole is the tail
of `int layout[20]`.

**L8 No BP frame without parameters and locals — VERIFIED** (r08, r11). `void f(void){...}` with no
locals has no `push bp`; `return 0;` is `sub ax,ax; retf`. `mov sp,bp` appears only when `sub sp` did.

**L9 The last call's argument pop may be dropped — VERIFIED** (r01, r04). If the next SP-sensitive
instruction is the epilogue `mov sp,bp`, `add sp,N` after the final call is omitted; with SI/DI to pop it is
kept (r05, r15).

**L10 The epilogue is replicated at every return — VERIFIED** (r09b, r06 `dense`). Each `return` gets its
own `mov sp,bp; pop bp; retf`; a `break` that reaches only the epilogue is also replaced by a copy.

**L11 Register-held CSEs never get homes — VERIFIED** (r04, r19). Call results and /Oa CSEs held in SI/DI
have no BP slot; only spills (L5) do.

## 2. Expressions and registers

**E1 Calls in a binary expression are evaluated right to left — VERIFIED** (r11, r15, r05).
`f(x) + g(x)` calls g first, parks its result in SI, then calls f and `add ax,si`. With four calls: SI, DI,
then a temporary (L5). Corroboration: parse_shape2d `*p++ = helper(dest) - helper(baseaddr)` keeps
helper(baseaddr) in SI:DI (miscc); build_track_object polar argument order (lsapply).

**E2 Far-pointer null tests: `or dx,ax` vs `or ax,dx` — VERIFIED** (r11). `if ((p = f()) != 0)` and
`if (p = f())` store then `or dx,ax`; `p = f(); if (p)` stores then `or ax,dx`. Corroboration:
init_audio_resources 0x270D2 exact with the assignment-in-condition form (permuter).

**E3 `unsigned char` against `int` compares unsigned — VERIFIED** (r11). `uc > i` → `mov al,[uc]; sub
ah,ah; cmp [bp+6],ax; jae`; `uc > 200` → `cmp byte [uc],0C8h; jbe`. Same under /Oal (tuflags3 mini/uc.c).

**E4 Signed chars: `a < b` is a byte compare, `(int)a < b` / `a - b < 0` are word compares — VERIFIED**
(r20b). Plain: `mov al,[sb]; cmp [sa],al; jge`; cast or difference: two `cbw`, `cmp ax,cx; jge` (REPORTED
source: s009c, whole seg009 TU).

**E5 Zeroing is `sub r,r`; long/far zero stores write the high word first — VERIFIED** (r11).
`lv = 0` → `sub ax,ax; mov [lv+2],ax; mov [lv],ax`. `xor` zeroing appears only inside intrinsic templates
(D3: strlen/strcpy/strcmp `xor ax,ax`; abs `cwd; xor ax,dx; sub ax,dx`). See U7 (S6).

**E6 Struct assignment uses `F2 A5` — VERIFIED** (r11). `r1 = r2` (12 bytes) → `push di; push si; mov di,
offset r1; mov si,offset r2; push ds; pop es; mov cx,6; repne movsw`. REPORTED (repfp): no size/model/flag
variant produced `F3` for movs/stos. See D3 for `F3 A6`.

**E7 Scalar long shift by constant is a CL loop — VERIFIED** (r11). `v << 4` → `mov cl,4; L: shl ax,1;
rcl dx,1; dec cl; jne L`. Unrolled shl/rcl pairs are not /O output (S5, hand-ASM SUPPORTED); /Os calls
`__aFlshl` instead (F4). Corroboration: accepted mmgr_get_res_ofs_diff_scaled 0x2A484.

**E8 32-bit products call the runtime — VERIFIED** (r11). `(unsigned long)a*b` → pushes `0,b,0,a`, far call
`__aFulmul`; `(long)a*b` → `cwd` each operand, `__aFlmul`. Inline MUL/IMUL into DX:AX is not /O output (S2).

**E9 Constant index offsets fold into the displacement — VERIFIED** (r11; `FACT-folded-constant-index`).
`chunks[i-16].v[1]` (76-byte elements) → `mov ax,4Ch; imul word [bp+6]; mov bx,ax; mov ax,[bx+chunks-4BEh]`
(-1216+2). Corroboration: sub_3771E 0x2771E, audio_init_chunk2 0x2764A. Acceptance of such addends needs the
reviewed stride+bound rule; this observation grants nothing.

**E10 /O forgets memory values after any indirect store and across calls — VERIFIED** (r19).
`dst->x = ...; dst->y = ...` reloads `mov bx,[bp+6]; mov si,[bp+8]` after the first store; three
`buf[limit+k] = ...` byte stores reload base and index each time; `draw(x1,y1+1); draw(x1+2,y1+1)`
recomputes `y1+1`. Register reuse across such a store in a target is an /Oa signature (F1), not a source
shape (miscc controls mini/a.c, mini/c.c).

**E11 `if (A || B) break;` ends the index CSE; nested conditions keep it — VERIFIED** (r10). Loop over
`tab[i]` (6-byte elements): `||`+break computes `si = i*6` for both tests but recomputes BX for the later
store; `if (A && B) store; else break;` stores through the same SI; two separate `if ... break;` recompute
BX every time. Corroboration: build_track_object nested `&&` (lsapply).

**E12 Condition shape selects temp registers and block order — REPORTED** (miscc, localsolver):
`for (;C1&&C2;step)` is laid out [step][C1][C2→step], `while (C1&&C2) S` as [C2][S][C1]; `&&` in a
for-condition changes the temp register (enter_hiscore 0x1A1C `for (i=0; t[i].marker<=score && i<7; ++i)`
gives the target DI temp); passing far-pointer table arguments instead of offset/segment pairs stops MSC
caching the table index in SI/DI (s009c).

**E13 ABS-macro shape shares one subtraction — REPORTED** (tuflags4 `build/workers/tuflags4/mini` r1, r3b,
r3d; register `FACT-abs-macro-shared-sub` SUPPORTED). `(a-b < 0 ? -(a-b) : a-b)` (or `a<b ? -(a-b) : a-b`)
compiles to `mov ax,[di]; cmp [bx],ax; jl J; mov ax,[bx]; mov bx,di; J: sub ax,[bx]` with BX kept;
`a>=b ? a-b : b-a`, commuted operands, if/else and casts reload. Treat that target sequence as an
ABS(a-b) signature (car_car_coll_detect_maybe exact in the seg001 TU).

**E14 Signed char comparisons need an int operand to widen — REPORTED** (s009c). `a - b < 0` or
`(int)a < b` forces an int compare where a plain `char < char` compares bytes; MSC 5.10 compares
`unsigned char` with `int` as unsigned (ja/jb) under /O and /Oal (tuflags3 mini/uc.c).

**E15 Fewer CSE temporaries under C2 symbol-table pressure — REPORTED, SUPPORTED** (s003c, s003d, s005c,
patterns, typenames; register `FACT-tu-extern-count-limits-cse`). update_frame (seg003) is exact only when
fewer distinct external/static symbols and literals are referenced before and inside it (mergesweep.py,
ctx16-ctx33). Correction of the first report: extern NAME LENGTHS do count. Cost model (s003d, typenames):
each referenced file-scope extern about 24 bytes plus its name length (arrays/functions about +4); file-scope
data DEFINED in the TU counts like a referenced extern; the current function's locals, parameters and labels
count; unreferenced declarations, struct field names, prototype parameter lists and earlier functions'
locals do not; data externs declared inside an earlier function (K&R block scope) are freed at its end. The
DOS pass directory and TEMP strings count too (D9). Consequences: whole-TU context and declaration
granularity are observable, and `blockdiff --stub-others` (which drops the other bodies' references) can
mislead near the limit; confirm on the complete TU.

**E16 CSE only between identical folded trees — REPORTED, SUPPORTED** (patterns R1, reproducer
`build/workers/patterns/scs/base_reg.c`; register `FACT-cse-tree-identity-char-cast`). /O forms a common
subexpression only when both occurrences have identical trees after front-end (C1) folding. Folded, so the
CSE survives: `(int)`, `(unsigned)`, `(short)`, `(unsigned short)`, `(long)` casts, parentheses, `(0,x)`,
`+0`, `|0`, `*1`, `&0xFF`, `%256`, `*(T*)&x` of x's own type. Not folded, so the CSE is suppressed:
`(unsigned char)x`, `(int)(unsigned char)x`, `(&x)[0]`, any indirection. No effect: `&x` taken elsewhere,
`volatile`, `register`, block scope. Calls and labels kill a CSE; indexed stores to named arrays do not.
The rule is sufficient, not necessary: setup_car_shapes (seg005) is exact either with one redundant
`(unsigned char)y_pos` (ruled acceptable with a review note) or, more naturally (blocklift, accepted in whole
seg005), by passing the value just stored (`word_40DF6[byte_4432A]`) instead of repeating the expression.

**E17 Pressure acts in non-monotonic bands — REPORTED, SUPPORTED** (patterns R2/R3, typenames; register
`FACT-tu-extern-count-limits-cse`). Codegen is a step function of the budget at the function, and the steps
recur: seg000 end_hiscore keeps its dead `pop si` for dP (name characters added) -14..+2 or +16..+1800 and
loses it for +4..+14 or -16 and below; loop_game (seg005) has CSE44 for relief 0-252, no CSE for 256-584 and
the target CSE42 for 588 or more; seg003 (semantic names) is exact for relief 520-880; seg001 is insensitive.
Budgets add across names at about one character per character (one pass-directory character is worth about
one name character). The temp slot follows the CSE set (R3): for a temp-slot residual with otherwise identical
code, scan pressure first. Because extern names are program-wide, one name per global must satisfy every
TU's band at once: seg000 needs a pressure floor while seg003/seg005 need a ceiling; the typenames plan C
(`build/workers/typenames/names_solution.json`, tools/namefit.py) satisfies all accepted TUs. Levers that are
natural source: shorter or spelled-out names within the measured margins, and K&R block-scoped externs in
the earlier function that alone uses them.

## 3. Control-flow layout

**C1 Switch = compare chain before the bodies, sorted by case value — VERIFIED** (r06, r08;
`FACT-msc-switch-dispatch-first`). Cases 4800h, 0Dh, 5000h, default → `mov ax,[bp+6]; cmp ax,0Dh; je;
cmp ax,4800h; je; cmp ax,5000h; je; jmp default`, then bodies in *source* order, each ending `jmp` join.
Case 0 tests with `or ax,ax`. The `cmp/jne/jmp` form seen in run_car_menu 0x1C42 is C3's far-target form.
REPORTED (lsapply, tu000g): the textually first case follows the chain; later cases that end in a jump are
emitted after the next unconditional jump; run_car_menu's arrow handlers are the last cases of `switch(key)`.

**C2 Jump table from 7 cases — VERIFIED** (r07, r07b). Dense 1..4/5/6 → compare chain; 1..7 → table; 8 cases
spread over 1..20 → table. Code: `sub ax,1; cmp ax,7; ja out; add ax,ax; xchg ax,bx; jmp cs:[bx+T]`; T is
word-aligned after the last body, one offset16 FIXUPP (own `_TEXT`) per word. Not an ASM marker (dlr).

**C3 Conditional jumps to far targets: `jcc-inverse +3; jmp near` — VERIFIED** (r23). 8086 Jcc is rel8
only: `cmp ax,20h; jne $+5; jmp near L` (`75 03 E9 ..`). Counts as one record-cut unit (O5).

**C4 Switch inside a loop — VERIFIED** (r06 `in_loop`, r08). `break` jumps to the loop head. With the switch
value stored in a local, the loop is rotated: `jmp head; [rest of chain + default code]; head: call; mov
[bp-2],ax; cmp ax,<lowest case>; jne rest; ...`. With `switch (getkey())` and no local the chain is not
rotated. `continue` vs `break` changes where the post-switch code lands.

**C5 Cross-jumping of identical tails — VERIFIED** (r08, r23, r02). In a switch whose arms end
`X(); g(5); break;` only the FIRST arm keeps `mov ax,5; push ax; call g; add sp,2`; later arms `jmp` into it —
identical with `goto done` instead of `break`. In an if/else-if chain the tail is kept once after the LAST
arm (it falls into the join). Merging works at instruction granularity (r23: `mov ax,63h; jmp` into the
middle of another arm's `push ax; call g; ...; retf`). Two identical if/else arms collapse and the test
disappears (r02). See §8 for the conflicting REPORTED rule.

**C6 A block that ends in `return` and is reached only by `goto` is emitted at the first goto site — VERIFIED**
(r09, r09b, r09c). `fail: h(); return 0;` placed at the function end compiles byte-identically to placing the
label inside the first `if`; a goto from inside a `for` loop pulls the block into the loop. If the block is
also reached by fallthrough it stays in place. Corroboration: detect_penalty 0x7816 return-block placement,
track_setup +0x33F (localsolver, lsapply). See §8 for the conflicting REPORTED rule.

**C7 An else branch that always leaves via goto/join is placed after the next unconditional transfer —
VERIFIED** (r20b; REPORTED s009c). `if (f(x)) {g(1);} else { if (g(2)) goto after; h(); goto after; } h(); after:
return g(3);` emits then-arm, shared tail and `retf` first; the else code follows the `retf` and jumps back.

**C8 `continue` in do-while jumps to the test — VERIFIED** (r20b). So a jump straight to the loop top
implies an explicit `goto`/other loop form (s009c).

**C9 Loop shape — VERIFIED** (r03, r21, r09b). `for (i=0;i<n;i++)` → init; `jmp test`; body; step; test at the
bottom with a backward Jcc. When the first test is provably true (`i=0; i<8`) the initial `jmp` is omitted.

**C10 90 pads before jump targets only where fallthrough is impossible — VERIFIED** (r06, r18; refines
`FACT-label-word-alignment` PLAUSIBLE). After `jmp`/`retf`, if the next label is odd one `90` is inserted
(sparse switch arms, `dense` case bodies). A loop head at odd 0009h reached by fallthrough gets no pad.
Corroboration: run_car_menu nops at +0x9BB/+0xBF3/+0xC21; its odd loop head +0x363.

**C12 Register state at a label follows the physically preceding code — REPORTED** (tuflags4 mini
g0/g1/g3/g5; register `FACT-label-state-physical-predecessor` SUPPORTED). At a join /O keeps a register's
known contents only if they also hold at the end of the code *physically* before the label, even when that
code ends in `jmp`. A block moved by C6/C7 (goto-only chains, else-arms leaving via goto) that does not load
the register forces a reload at the later join; removing redundant gotos restores the reuse
(update_car_speed exact after `goto done` was removed; the former symptom S9).

**C13 Loop test placement follows what comes after the loop — REPORTED** (supervisor note, integ25): a loop
followed by a jump is laid out with the test at the top; a loop followed by a label keeps the test at the
bottom (C9 form). A `jcc` to a lone `jmp inc` at a for-body end comes from `if (!(C)) continue; return 1;`
(unthreaded); `if (C) return 1;` threads the jump (tuflags4).

**C11 Big-function block placement is still partly unexplained — REPORTED**: run_car_menu pre-case block
(+0x9BC), track_setup +0x163 post-switch block, loop_game 0x13B4C mode order (tu000g, lsapply, loopg).

## 4. Declarations, keywords, pragmas

**D1 `far` binds per declarator — VERIFIED** (r12). `extern struct S far *pa, *pb, *pc;` makes only `pa` far
(`les bx,[pa]`); `pb`/`pc` are near (`mov bx,[pb]`). Write one declaration per far pointer.
Corroboration: mouse_draw_transparent 0x18E04 (accepted after this fix, s008harvest).

**D2 `#pragma intrinsic(inp, outp, _enable, _disable)` under /O — VERIFIED** (r13, r13a;
`FACT-pragma-intrinsic`). Prototypes first, then the pragma: `cli; in al,60h; sub ah,ah; ...; mov ax,20h;
out 20h,al; sti`. Pragma before the prototype → `C2164: intrinsic was not declared`. Direct IN/OUT/CLI/STI
are therefore not ASM markers by themselves.

**D3 Memory/string intrinsics exist under /O too — VERIFIED** (r13b, r13f, r13g). With the pragma *after*
the prototype, canonical /O inlines `memcpy` (`shr cx,1; repne movsw; adc cx,cx; repne movsb`), `strlen`,
`strcpy`, `strcat`, `strcmp`, `memset` (`repne stosb`) and `abs`. `strcmp`/`memcmp` expand to
**`F3 A6` (`repe cmpsb`)**, also under plain `/Oi` without a pragma. Since integ25, `tools/preprocessor.py`
admits `#pragma intrinsic(...)`/`#pragma function(...)` for the MSC 5.10 intrinsic set (manual list:
memcpy, memset, memcmp, strlen, strcpy, strcat, strcmp, strset, abs, labs, _rotl, _rotr, _lrotl, _lrotr,
inp, outp; README.DOC additions: inpw, outpw, _enable, _disable and the floating-point names). Each name is
verified inline with the pinned compiler (tests/test_integ25.py) and frozen in the recipe closure.

**D4 `int strlen(char*)` conflicts with the /Oi intrinsic — VERIFIED** (r13c, r13d). Under /Ox or /Oi:
`C2086: 'strlen' : redefinition`; the standard `unsigned strlen(const char*)` compiles. Under /O both call
`_strlen` far. seg032 (read_line 0x2A4B6) declares `int strlen(char*)` — its signed compare needs it — and
has 8 far `_strlen` calls, so it is not /Oi (tuflags3).

**D5 Inline strlen template — VERIFIED** (r13d .Ox/.Oi): `push di; push ds; pop es; mov di,s; mov cx,0FFFFh;
xor ax,ax; repne scasb; not cx; dec cx`. Target sign of /Oi or the pragma. Corroboration: audio_load_driver
0x278CA, audio_make_filename 0x29CCE (seg027/030, `TUFLAG-Ox-seg027`, `TUFLAG-Ox-seg030`).

**D6 `_loadds` equals `/Au` — VERIFIED** (r14, r14b). Both emit `push ds; mov ax,DGROUP; mov ds,ax` (base16
FIXUPP) ... `pop ds` with identical bytes and fixups. Every seg028 function has it (`TUFLAG-Ox-seg028`).

**D7 31 significant identifier characters — VERIFIED** (r16). Longer names warn C4011; the public is `_` +
31 characters (32 in total), e.g. `_audioresource_compare_chunkname`. MASM truncates differently (31 total;
see NOTES integ24).

**D8 `static` functions — VERIFIED** (r15). Emitted as a local public (LPUBDEF B6, no underscore) and still
called with `push cs; call near` (O3).

**D9 The pass directory and TEMP strings share the symbol-table budget — VERIFIED** (integ27,
`FACT-compiler-memory-environment`, `FACT-tu-extern-count-limits-cse`). CL starts each pass with its own
environment `MSC_CMD_FLAGS=-il <TEMP>\NNNNNN -ef <pass dir>\c23.err ...`, `NO87=`, `;C_FILE_INFO` plus the
pass path; the user's environment is not passed. C2's output depends on the length of these strings near
the budget limit: whole seg000 keeps end_hiscore's dead `pop si` only for a DOS pass directory of 5-22 or
>=35 characters (TEMP `.`), whole seg003 (short names) only up to 38, seg005 up to ~50
(`build/workers/integ27/mem/sw_path*.log`). Free memory had no effect from ~316 KB to ~660 KB per pass.
The pinned profile therefore fixes the DOS pass directory (`C:\TOOLS\MSC-5.10`) and TEMP (`.`) in
`layout/toolchain.json`; the DOSBox-X crosscheck mounts a mirror at the same DOS path. For research,
compile only through `tools/compiler.py` (other paths or TEMP values can change pressure-limited code).

## 5. Object and OMF emission

**O1 Every function is word-aligned with a `90` pad — VERIFIED** (all repros; `FACT-odd-length-padding`).
An odd-length function is followed by one `90`, also mid-object. Exception: /Os emits no pads (F4).

**O2 Odd entry without preceding 90 ⇒ not an MSC /O function start — REPORTED/derived**
(`FACT-odd-entry-non-msc`). Examples: audio_op_unk/audio_function2 (asm-e); oddity security_check 0x44CF
follows end_hiscore with no pad although main reaches it by `push cs; call near` (objmap; open).

**O3 Same-TU far callees: `push cs; call near` — VERIFIED** (r15). Callees defined earlier, later
(even only implicitly declared) or `static` get `0E E8 rel16` with a self-relative offset16 FIXUPP to the
public; external callees get `9A` far calls. A caller with such calls can only be accepted with its callees
(whole-object group). Corroboration: timer_get_delta_alt 0x1A230; seg008 closure (inputtu).

**O4 FIXUPPs are emitted in descending location order within each LEDATA — VERIFIED** (r06, r17). Hence one
recompiled object yields one descending relocation run per record; a target run that restarts means a record
cut, i.e. object context (`FACT-relocation-order-reveals-object-context`, REPORTED relorder2; s008harvest
mouse_draw_group).

**O5 CODE LEDATA cut: ≥944 bytes or the 99th FIXUPP — VERIFIED** (r17, r17b; `FACT-ledata-split`). 120 far
calls: records [0,495) with 99 fixups, then [495,602). 3-byte-instruction body: records of 945, 945, 516
bytes (closes after the instruction reaching ≥944; hard cap 949). REPORTED (objmap, 4,487 records): a
jcc+3/near-jmp pair and each switch-table word are single units; cuts count from the object start; EXTDEF
first use does not flush (`HYP-extdef-first-use-flush` FALSIFIED). Tool: `tools/cut_simulator.py`.

**O6 `_DATA` order: literals first, then initialised globals, word-aligned with holes — VERIFIED** (r20a).
`"xyz"`,`"hello"` (order of appearance) at 0..9, `char c1` at 10, hole 11, `int w1` 12, `char name[3]` 14..16,
hole 17, `int w2` 18. Holes are simply not written; `object_probe.read_object` rejects them ("Holes or
overflow") unless the recipe carries the reviewed `sparse_zero` `initialized_ranges` policy (single-byte
holes before word-aligned offsets only; integ25). The literal pool comes first even in whole-object groups
(s009c, seg009 TU _DATA 0x34B0; seg034, seg032 string pools).

**O7 /Ol-family object tail — VERIFIED** (r22 = tuflags' wheel_oa.c). Under /Oal the 104-byte function is
followed by `90 90` and SEGDEF length 107 vs 106 LEDATA bytes (one declared, unwritten byte); under /Oa the
object is 106 bytes. Image slivers such as seg029's `90 90 00` before seg030 are such tails (tuflags3); a
group recipe owns them as a reviewed `object_tail` (`msc-code-object-tail-v1`) with the CODE `sparse_zero`
prefix (integ25, obj_seg029 505 B).

**O8 Single `00` between an odd procedure and the next is LINK fill between objects — REPORTED**
(`FACT-00-pad-object-boundary`); `FACT-c-segment-one-object`: each C code segment is one object (objmap).

**O9 Data-before-code FIXUPPs can split one TU's relocations across EXEPACK banks — REPORTED** (relorder).

**O11 The pinned LINK keeps FIXUPP order in the MZ table — VERIFIED** (integ25, `tools/link_order_probe.py`,
tests/test_link_order.py; LINK 3.65 and 3.61). A MASM 5.10 module's ascending FIXUPPs (several LEDATA records,
far CALLs and `SEG` words) stay ascending; an MSC object's descending FIXUPPs stay descending. EXEPACK then
regroups entries by 64 KiB bank, stably. So a target region whose relocations form descending runs per
record was produced by a translator emitting descending FIXUPPs (MSC, O4), not by the pinned MASM 5.10: the
seg007 audio module, frame_callback_replay_group and seg037's expandedsize show such runs, seg012 ASM modules
ascending ones. One descending run crossing a proposed object boundary proves one record, hence one object
(seg003 at 41204, integ25).

**O12 Zero tails of partially initialised aggregates are LIDATA - VERIFIED** (integ26, tests/test_integ26.py). `char big[82] = {0};` emits one LEDATA byte for the explicit initialiser and a LIDATA record repeating the zero for the remaining 81 bytes. The object needs no explicit zero list (seg000 `byte_3B80C`/`byte_3B85E`); research compiles count the LIDATA expansion as initialised coverage, so only word-alignment holes remain.

**O13 `extern` -> file `static` (own `_BSS`) leaves /O code generation unchanged - VERIFIED** (s009bss, integ35). Changing seg009's six file-scope far-pointer objects from `extern` to `static` definitions in the object's own `_BSS` produces identical instructions, fixup locations, address folding and relocations. Only the FIXUPP target changes: an EXTDEF with an F5 frame becomes the `_BSS` segment with an F1 DGROUP frame, and the static's offset is added to the LEDATA addend (98 sites, all five members exact). The object stayed exact for name-length changes of -115..+65 characters, so seg009 lies outside its pressure band (`FACT-tu-extern-count-limits-cse`). The static-to-offset order within `_BSS` follows the identifier-spelling hash (L8-msstatic, L9-mschash; diagnostic model `tools/msc_static_model.py`).

**O14 Static `_BSS` placement is flushed per function definition - VERIFIED** (s005bss, integ37; fixtures `tests/fixtures/msc510_static_flush_fixtures.json`). File-scope statics stay pending until the next function *definition*; prototypes, `extern` declarations and initialised data definitions do not flush them. At the definition the pending set is emitted in file-static hash order (bucket sum(byte & 0xDF) mod 256 ascending). Then the function's block statics follow, block by block: each block's own statics by the local-slot hash sum(byte) & 15 ascending, then its nested blocks (a nested block follows its enclosing block although its bucket may be lower; sibling blocks keep source order). Ties in both hashes go newest declaration first. Statics still pending at the end of the file are emitted last, and unreferenced statics are allocated like referenced ones (and cost symbol-table pressure). Ten compiled fixtures (e1-e10) and seg005's complete 27-static `_BSS` (replay_control flushed at `replay_unk2`, the 17 `setup_car_shapes` block statics, then the nine statics flushed at `loop_game`) are reproduced by the diagnostic `msc_static_model.layout` (`bss_link.static_layout`). A single flush group reduces to the O13 file-static order (seg006: all 30 statics precede its first definition).

**O10 MSC 5.00 is byte-identical on these idioms — VERIFIED** (r08, r11 with `msc500-medium`;
`CC-MSC500-same-flags` NON_DISCRIMINATING).

## 6. Optimisation-flag signatures

Canonical `/O` is fixed for normal work; only registered TUFLAG objects use another set (NOTES ruling).
Signatures help recognise such objects; they never justify changing flags for one function.

**F1 /Oa (no aliasing) — VERIFIED** (r19; `TUFLAG-Oa-seg023-025`, S8). Pointers stay in BX/SI across stores
(`fill` 62→56 B); `buf+limit` computed once into SI (40→26 B); `y1+1` held in SI across a far call
(`push si` twice). REPORTED (tuflags2): struct-copy words and negated subexpressions are also CSE'd across far
calls; `/Oa` code still reloads parameters when SI/DI are consumed by other CSEs. Objects: seg014/015/019/
021/022/023/024/025/032 (preRender_* 0x26246.., mat_rot_x 0x26F2A, sub_3702E 0x2702E, read_line).

**F2 /Ol, /Oal — VERIFIED** (r21, r22; `TUFLAG-Ox-seg027/028/029`). `for (i=0;i<8;i++) voices[i].a = 0;`
→ `mov si,offset voices; mov cx,8; L: mov [si],0; add si,4Ch; loop L` plus the exit value stored to the
counter home (`mov [bp-2],8`) and extra frame words (sub sp 2→6), under /Ol and /Oal alike. A far byte-copy
loop becomes `repne movsw` + `movsb` with pointer write-back (`add [bp+6],ax`) only under /Oal and /Ox.
REPORTED (tuflags3): /Ol ranks registers by use (`if (n) do {...; n--;} while (n)` → SI; `--n` in the
condition → DI); mismatched char/unsigned char element types keep byte loops; a comma-expression increment
materialises its value (`mov ax,si; mov dx,ds`).

**F3 /Ox = /Oailt — REPORTED** (tuflags). seg027 needs both the inline strlen (D5) and a backward `loop`
(audio_load_driver); `/Oal` + a strlen pragma is equivalent (NON_DISCRIMINATING).

**F4 /Os — VERIFIED** (r06, r11 `.Os`): no function pads (publics at odd offsets), a shared epilogue reached
by `jmp` instead of L10 copies, long shifts via `__aFlshl`. Quick discriminator for pads (tuflags).

**F5 /Ot, /On, /Op equal /O — VERIFIED** (r06, r11) and on all 76 controls (tuflags, tuflags2).

**F6 Per-object map — REPORTED**: see `TUFLAG-*` entries and `tools/object_flags.py`. Early objects
seg000-009/031 and seg034 are canonical /O (`TUFLAG-sweep-2026-09-26`, `TUFLAG-Ol-seg034` FALSIFIED).

## 7. Open, unexplained residuals

All REPORTED; each is a place where no rule above is known to apply.
- **U1 S1 pad_id 0x16C06**: SI counter, no home, no flag set explains it; seg007 whole object accepted as ASM.
- **U2 S7 seg008**: mouse_timer_sprite_unk `cmp mem,di` vs `mov ax,di`; get_super_random register choice;
  sprite_blit_to_video `[bp+0Ah]` vs `sub si,si` — no flag set moves them.
- **U3 DI-only prologue**: no source shape producing a DI-only save is known (regrule).
- **U4 enter_hiscore 0x1A1C**: DI vs SI temporary at +0x47 (tu000g).
- **U5 Char argument `mov al,4` vs `mov ax,4`** (seg037), flag-insensitive (tuflags2).
- **U6 handle_ingame_kb_shortcuts 0x123FA**: target `jbe` vs candidate `jle`; an unsigned parameter did not
  change the candidate (obj-s005).
- **U7 S6 `xor` zeroing** (sub_307D2, timer_reset, ...): canonical zeroing is `sub`; `xor ax,ax` occurs only in
  intrinsic templates (E5/D3), which does not by itself explain these targets.
- **U8 sub_2298C 0x1298C**: target reuses ES=SS for a second stack copy. D3 shows the memcpy intrinsic is
  available under /O (pragma after prototype); L-s005 tested it against this target and it does not match
  (the struct copy form, not memcpy, remains the lead).

## 8. Contradictions and corrections

Found while reproducing; the register is unchanged (supervisor is its single writer).
1. **memcpy intrinsic "unavailable" (obj-s005) / "exists only with /Oi" (tuflags2)**: contradicted. `C2164`
   means the pragma preceded the prototype; after the prototype, canonical /O inlines memcpy and the string
   functions (D3). The preprocessor tooling limit was removed by integ25 (D3).
2. **`MARKER-F3-string-op` (SUPPORTED)**: partly contradicted. MSC 5.10 intrinsic `strcmp`/`memcmp` emit
   `F3 A6` (`repe cmpsb`) under /Oi or the pragma. No F3 `movs`/`stos` was produced (F2 A4/A5/AA only), so the
   marker still holds for `F3 A4/A5/AA/AB`, not for `repe cmps`.
3. **S6 "xor zeroing from no MS compiler" (msc6disc)**: qualified: MSC 5.10 intrinsic templates contain
   `xor ax,ax` / `xor ax,dx` (D3, E5); ordinary zeroing is still `sub`.
4. **Cross-jumping "break keeps the later copy, goto the earlier" (lsapply)**: not reproduced; in simple
   switches both keep the first arm's copy (C5). Treat as context-specific.
5. **"A label at the function end stays at the end" (miscc)**: not reproduced; a goto-only return block moves
   to the first goto site wherever it is labelled (C6). Treat as context-specific.
6. **"/Ol without /Oa produces none of the loop traits" (tuflags)**: source-specific; /Ol alone emits `loop`,
   a strength-reduced SI pointer and the exit store for a global-array loop; the far copy → `rep movsw`
   conversion needs /Oa as well (F2).
7. **`FACT-label-word-alignment` (PLAUSIBLE)**: supported with a precise scope — pads only after unconditional
   transfers (C10).
9. **"S9: AX/BX kept across a ternary join is a compiler symptom" (physc)**: explained as source shape — the
   ABS-macro operand order (E13) and goto-induced block motion (C12) reproduce both residuals (tuflags4).
8. **Switch dispatch "cmp/jne/jmp per case"** (register wording): plain chains are value-sorted `cmp/je`; the
   `jne/jmp` pair is the far-target form (C1, C3).

**O15 Tentative definitions: pressure, record order and block externs - VERIFIED** (integ40; the communal publication, `build/workers/integ40/bisect_conv.py`, `hashpos.py`). A file-scope tentative definition `T x;` emits a near COMDEF (62h) instead of an EXTDEF; referencing code and FIXUPPs are unchanged.
- **Pressure.** Defining in place costs more C2 symbol-table pressure than an `extern`. In seg001 and seg003, turning their file-scope externs into in-place definitions changes the generated code (seg001: -24 bytes in `update_player_state`'s region; seg003: -6 bytes). Bisection shows a cumulative effect, with no single culprit. The same definitions placed after the last function (with the externs kept) leave every byte unchanged. seg000, seg004, seg005, seg006, seg008 and seg009 take in-place definitions (including unreferenced neutral ones) without change.
- **Record order.** In these objects, EXTDEF, COMDEF and PUBDEF record order is the compiler's symbol-table walk. It depends on the spellings and is not source order: a definition appended at the end of seg001 is its 38th name record, and renames reorder PUBDEFs. Small probes happen to show declaration order. (This corrects the L9-mschash note "record order = source order" for real TUs.) So the LINK first-sight position of a communal inside its first module is spelling-dependent; the real link is the authority.
- **Block externs.** A block-scope `extern` followed by a file-scope definition emits both an EXTDEF and a COMDEF for the name. LINK and the acceptance probes treat the pair as one communal.
