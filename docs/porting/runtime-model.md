# Runtime execution model for an SDL3 port

> **Correction to the earlier P2c draft:** `.DRV` entry points are called through direct far-call vectors at offsets in the loaded driver image, not through INT 60h-62h. P3a established this from the callers and all four driver images; the observed offsets and unresolved callee semantics are detailed in [audio-drivers.md](audio-drivers.md).
>
> **Editor correction:** P3c found the complete in-executable track editor in `obj_seg009`; see [modes-and-editor.md](modes-and-editor.md).

## Evidence convention and scope

Paths below are workspace-relative. The cited C under src/ is the project’s accepted reconstruction for the named member; where a matching image boundary is known, its segment offset is given as an additional anchor. Accepted assembly services and image addresses are called out separately. The Restunts checkout is used only as a semantic lead where noted. Negative findings mean “not found on the inspected accepted call path,” not proof that no other executable or data file implements the behavior.

This is an executable-scoped model of the Stunts 1.1 MCGA game. The in-game track chooser, preview, and built-in track editor are in scope. A later source-backed pass in [modes-and-editor.md](modes-and-editor.md) supersedes the earlier P2c draft claim that the editor was absent: `obj_seg009` contains the editor loop called from the track menu. The game checks for `tedit.*` on a separate idle/demo path; that check is not evidence that the editor itself is a separate executable. Dynamic DOSBox-X observations, capture conditions and explicit measurement gaps are collected in [runtime-measurements.md](runtime-measurements.md); they are kept separate from source/image-derived behavior below.

## Execution map

| Phase or state | Observed behavior | Evidence |
|---|---|---|
| DOS entry | The image entrypoint maps to load offset 0x1CC62. RT-CRT0-startup attributes the startup block to pinned MSC 5.10 CRT0.ASM; see the qualification below. | evidence/toolchain-hypotheses.json, RT-CRT0-startup; toolchain/msc510/STARTUP/DOS/CRT0.ASM:160-276 |
| Game initialization | main calls initialize_main(argc, argv), then loads main data/fonts, allocates the track/replay arena, initializes the Kevin PRNG, and enters the menu loop. | src/obj_seg000.c:424-430, 446-500, 520-530 |
| Menu routing | run_menu result 3 opens track selection, 2 opponent selection, 4 options, 1 car selection, and 0 starts a game. The options path may set up a loaded replay before entering run_game. | src/obj_seg000.c:530-554; src/obj_seg005.c:2129-2170 |
| Live start | run_game initializes a live session in mode 1 (waiting/pre-start), then the first mouse or keyboard/joystick 0x30 action bits change it to mode 0 (live/recording). | src/obj_seg005.c:619-656, 865-873 |
| Playback | A default replay can be selected for the idle/demo path; a selected replay sets mode 2, restores or advances state, and reads recorded input events rather than physical input. | src/obj_seg000.c:574-578; src/obj_seg005.c:609-618, 661-675, 1099-1110 |
| Exit/return | run_game removes its frame/audio callbacks, then the outer main loop returns to menu, intro, or high-score handling according to timeout and result state. | src/obj_seg005.c:885-910; src/obj_seg000.c:594-624 |

The numeric gm_playmode values observed in this path are 0, 1, and 2. The source assignments and branches support the labels live/recording, waiting/pre-start, and replay/playback above; these are reconstructed semantic labels, not an enum declaration.

## DOS startup, configuration, and subsystem selection

The MZ header in build/references/restunts/stunts/game.exe has CS:IP 1CC5:0012, resolving to load offset 0x1CC62; this agrees with RT-CRT0-startup in evidence/toolchain-hypotheses.json. The compact toolchain hypothesis RT-CRT0-startup is SUPPORTED: 197 startup code bytes match pinned 5.10 dos/crt0.asm outside 24 fixup spans, and 11 relocation sites align with the oracle. The same hypothesis explicitly says the full DATA and STACK contributions and some header details are not proven. Accordingly, CRT0.ASM is an image-supported attribution for this startup block, while its source supplies the sequence: establish DGROUP stack and free-memory limit, release excess DOS memory, zero BSS/common, set DS, call _cinit, parse environment and argv, call main(argc, argv, envp), then call exit with main’s return value (toolchain/msc510/STARTUP/DOS/CRT0.ASM:160-276). Do not treat all pinned CRT data/stack facts as independently image-verified.

main immediately calls initialize_main (src/obj_seg000.c:424-430). The accepted initializer performs this observed order:

1. Installs the keyboard interrupt path, shift check, character callback, and shortcut callbacks (src/obj_seg031.c:94-107; image keyboard handler seg012:0x208C6-0x209A5).
2. Initializes display globals and allocates the A000 video region (src/obj_seg031.c:109-120). It sets g_videoflg5 to zero at line 117.
3. Parses argv tokens beginning with slash. /h selects video mode 4; /sXY selects the two-character audio driver code; /sSB aliases to code ad; /ns disables two audio flags; /nd is parsed into an unused local and has no effect in this path (src/obj_seg031.c:122-158). The default code is pc15 (src/obj_seg031.c:28).
4. Selects BIOS video mode 13h, optionally applies mode 4, sets up the timer interrupt, clears/copies the initial sprite page, initializes the mouse for 320 by 200, then loads the selected audio driver. A failed load exits with status 1 (src/obj_seg031.c:160-181). There is no dynamically selected video-driver blob in this path.
5. Installs the critical-error handler, loads the display palette and cursor sprites, measures drawing cost, chooses the frame rate and detail level, waits on a video-status transition for a bounded loop, and stirs both rand() and the Kevin PRNG (src/obj_seg031.c:183-267, 270-302).

The video selectors are BIOS calls: video_set_mode_13h uses INT 10h/AL=13h (seg012:0x23816-0x23848; asm/video_set_mode_13h.ASM:18-24); video_set_mode4 is a separate path (seg012:0x2005E-0x200B6). Startup palette upload calls video_set_palette(0, 0x100, colors) after reading the !pal entry from SDMAIN and skipping 16 bytes; it copies exactly 0x300 bytes (src/obj_seg031.c:270-285; BIOS INT 10h AX=1012h in asm/video_set_palette.ASM:12-24). The 256 RGB triplet interpretation follows the BIOS service and byte count. No palette change was found in the accepted race render/update path.

No INI/config file parser appears in the accepted main/initializer path inspected. Game settings are held in GAMEINFO and are populated by menus and replay loading; a replay loads its 26-byte header into the settings structure (src/obj_seg005.c:1267-1276). Menu settings and command-line switches must therefore remain separate inputs in a port.

After initialize_main returns, main loads main data plus fontdef.fnt/fontn.fnt, allocates one 0x6BF3 track-data arena with named map/replay/high-score views, and calls initialize_kevin_random("kevin") (src/obj_seg000.c:446-500). It then runs intro/menu/track setup and the game loop (src/obj_seg000.c:520-624). Runtime resources visible on these paths include main, fontdef.fnt, fontn.fnt, SDMAIN palette/cursors, car resource files, engine resources eng1 and eng, track .TRK files, replay .RPL files, high-score .HIG files, and audio .DRV/.KMS/.DSF/.SFX/.DVC/.VCE resources. The engine resources are loaded in vehicle setup (src/obj_seg005.c:1616-1665); menu audio resources are requested at src/obj_seg000.c:529, 636, 2068-2070; the accepted audio loader tries dsf/sfx effects, kms songs, and dvc/vce voice resources (src/obj_seg027.c:521-587).

## Menus, race, playback, and editor boundary

The top-level menu dispatch is not a single “race mode”: it routes to track selection, opponent selection, options, and car selection before track setup and run_game (src/obj_seg000.c:526-554). When the idle/demo branch tests file_find("tedit.*"), zero routes to intro and nonzero routes to default replay (src/obj_seg000.c:574-578); the Restunts semantic helper says file_find returns NULL on no hits (build/references/restunts/src/restunts/c/fileio.c:254-257), so that branch treats a missing matching file as intro. The accepted track chooser loads a resource named tedit only for button labels, draws a 3D track preview, and offers .TRK selection/restart/exit actions; it does not edit map cells (src/obj_seg000.c:893-956, 1009-1035). Thus “editor” here is not an additional gm_playmode. The editor loop is in the accepted executable; TEDIT.PRE and SDTEDIT.PES supply resources used by that path (see modes-and-editor.md).

run_game states:

- Mode 1 initializes the selected live car/session and waits. Mouse buttons or the keyboard/joystick 0x30 action bits transition it to mode 0 and reinitialize game state (src/obj_seg005.c:645-660, 865-873).
- Mode 0 runs live input, records one control byte per logical frame, and runs the physics/update kernel.
- Mode 2 plays a recorded stream. The playback branch advances tmr2 only up to game_recordedframes; seeking restores a saved GAMESTATE snapshot and replays forward from that point (src/obj_seg005.c:661-675, 1099-1110; src/obj_seg001_complete.c:1671-1695). Slow/fast replay UI also adjusts callback scheduling in frame_callback (src/obj_seg005.c:1045-1077).

The .TRK selector reads the selected file into td14tb; the accepted arena places the adjacent td15p_9 map view 0x385 bytes later (src/obj_seg000.c:479-495, 1013-1022). The accepted source does not pass an explicit byte count to file_read_fatal, so treat 0x70A as the observed two-plane map extent, not a checked maximum-read argument. High scores are per-track .HIG files and read/write 0x16C bytes (src/obj_seg000.c:1038-1068, 1214-1225). Replay load reads the whole .RPL into the contiguous arena and copies GAMEINFO from its first 0x1A bytes. Replay save writes recorded-frame-count plus 0x724 bytes (26-byte header + 0x70A-byte map + event bytes) (src/obj_seg005.c:1267-1291). This agrees with the executable format evidence in docs/porting/formats.md:24-32. Track setup derives runtime track/physics data from map/resource inputs before simulation.

## PIT timer, callback counters, and simulation pacing

The accepted timer setup listing’s default entry loads DX=0x2E9C (decimal 11,932), writes control byte 0xB6 to port 43h, saves/replaces the INT 8 vector, then writes DL and DH to port 40h (asm/timer_setup_interrupt.ASM:7-13, 41-76; timer setup image range seg012:0x201A0-0x20268). The interrupt listing decrements word_3F886 and reloads it from word_3F884 on expiry; each 5-IRQ rollover increments word_3F87C and carries into word_3F87E. When callback suppression word_3F88E is zero, every IRQ increments the 32-bit _timer_callback_counter and scans the four-byte far callback table. Reentry is guarded by byte_3F88C; reentrant hits increment word_3F88A and update its high-water value word_3F888. When byte_3F881 is nonzero, the rollover path also calls the sound countdown helper (asm/timer_intr_group.ASM:37-84; image range seg012:0x20329-0x203BA).

The control-word mismatch leaves the effective channel-0 mode unresolved. In the 8253/8254 encoding, 0xB6 selects channel 2 with low/high-byte access and mode 3, while port 40h is channel-0 data; this sequence therefore does not explicitly set channel 0's control word. A DOSBox-X harness using the exact sequence observed 93 INT 1Ch callbacks during a nominal one-second wait; a channel-0 control harness logged 99.9985 Hz for divisor 11932. Those approximate harness results support a roughly 100 Hz interpretation but do not prove the original run's channel-0 mode or rate. See [runtime measurements](runtime-measurements.md) for methods and limits; do not hardcode 100 Hz as an established game tick rate.

At the game layer, initialize_game_state sets g_cvxintvl = rate_frame * 30 and frmcs_time = 100 / rate_frame (src/obj_seg001_complete.c:1552-1553; declarations/comments in src/obj_seg005.c:284-285). Startup benchmark selects rate_frame 20 if timerdelta3 < 75, else 10, and chooses detail level by thresholds 35/55/75/100 (src/obj_seg031.c:221-249). For a fresh live game, the benchmark selects a 20 or 10 logical simulation frame target per nominal second; playback takes the rate from the replay GAMEINFO (src/obj_seg005.c:643-665, 2168-2169). The initialization selects a matching steering-response table and snapshots state every 30 simulation seconds; the 100 divisor in frmcs_time reflects the engine’s timer-unit assumption, not independently verified PIT frequency (src/obj_seg001_complete.c:1545-1553).

set_frame_callback registers frame_callback in the timer callback list and zeros g_clocks (src/obj_seg005.c:1003-1007). The callback increments snd_tick_clock on each timer callback, decrements timeraud when simulation ticking is allowed, reloads timeraud from frmcs_time at zero, increments g_clocks, then calls replay_unk2 to advance logical input/playback ticks; sigframe, replay flags, and replay UI modes gate or alter that path (src/obj_seg005.c:1021-1082). In live race, the foreground loop repeatedly calls update_gamestate while core.game_frame differs from tmr2, then renders (src/obj_seg005.c:677-684, 784). If the foreground is late, it performs multiple logical updates before its next render. This is the source-supported “catch up, then draw” rule. Menu/dialog and post-race callbacks have their own behavior; the 10/20 target applies to this game loop, not every UI animation.

The PIT ambiguity does not invalidate the counter divisions visible in code. Preserve the implemented divider/counter logic and the 5/10 callback countdown while measuring the modern SDL clock source independently. The current DOSBox-X run still lacks guest-level I/O events; confirming the live channel-0 mode/rate needs port instrumentation or a target-configuration measurement (see [runtime measurements](runtime-measurements.md#explicitly-unmeasured)).

## Input sampling, normalization, and replay capture

The keyboard interrupt handler maintains key-down state from scan-code make/break (high bit distinguishes break) and feeds a character ring; its image entry is seg012:0x208C6-0x209A5. get_kb_or_joy_flags maps the configured scan codes in kbinput to action bits and calls the joystick poll only when no configured key bit is active (seg012:0x20538-0x205C8). The joystick helper polls port 201h and applies axis-extrema/hysteresis logic before returning flags (seg012:0x205FC-0x207B4). The game also samples DOS mouse state via mouse_get_state. No separate gameplay debounce timer was found: keyboard uses make/break state, mouse is polled, and joystick filtering occurs in its hardware polling path.

replay_unk2 is the logical input sampling/record point:

- In mode 2 it consumes the replay timeline without consulting physical controls.
- For mouse steering it centers X around 0xA0 (160), applies an 0x12 (18-pixel) dead zone, and maps the two mouse button tests into bits 2 or 1.
- For joystick mode it obtains replay_axis_value, converts the signed result through replay_axis_magnitude, and takes the digital portion from get_kb_or_joy_flags() & 0x33.
- Otherwise it polls get_kb_or_joy_flags directly. Scan codes 0x1E and 0x2C add bits 0x10 and 0x20.
- Mouse/joystick analog position is kept temporarily in a 64-entry ring. Immediately before the simulation step, replay_unk compares the steering target with current steering using a speed-indexed table and may OR event bit 4 or 8 into the current replay byte. The raw analog axis is not stored in the .RPL stream.
- The final event byte is written at g_tdreplay16buf[tmr2++] and recorded-frame count is incremented. At the 12,000-frame buffer limit, the code slides a 30-second block and associated snapshots forward rather than extending the buffer (src/obj_seg005.c:1090-1177, 1734-1770).

Keep the control byte opaque at the SDL input boundary until its complete bitfield is independently mapped. Preserve polling/sampling once per logical frame, not once per render. Keyboard/mouse interaction can also control menus, playback, and pause; those UI checks are separate from the simulation event capture above.

## Deterministic simulation and replay requirements

update_gamestate reads the event byte for core.game_frame, latches the active-input state, optionally snapshots, increments game_frame and its elapsed-frame counter, then (when active) calls player_op, optional opponent_op, update_camera_target, optional update_crash_debris, and audio_carstate, in this order (src/obj_seg001_complete.c:1698-1743). player_op updates speed with update_car_speed, applies the event steering field through upd_statef20_from_steer_input, then updates grip and player state, adds travel distance, and enters penalty/lap/collision handling (src/obj_seg001_complete.c:1775-1825 and following body). Those callees, their lookup tables, and their integer widths/overflow behavior are part of the deterministic simulation closure. The image anchors for update_gamestate and player_op are seg001:0x7008-0x71E8 and 0x71E8-0x7816; opponent_op is seg001:0x4712-0x4D6C.

The executable uses fixed-point/integer code rather than wall-clock delta or floating-point physics in this path. Preserve signedness, word/long widths, wrap points, arithmetic shifts, and expression order. World positions are kept in 64-scaled units in setup and converted with arithmetic right shifts in render/update paths (src/obj_seg001_complete.c:1649-1656; src/obj_seg003.c:918-927). Trig comes through sinfast/cosfast and lookup tables; image anchors are seg012:0x226DE-0x22738. Do not substitute host floating-point trig or recompute fixed tables from idealized values.

The custom Kevin PRNG has a six-byte mutable seed. get_kevinrandom adds seed byte 5 into byte 4, then cumulatively into bytes 3..0 modulo 256, increments byte 5, and cascades the increment through earlier bytes on carry; it returns byte 0 zero-extended (asm/obj_seg002.ASM:196-230). initialize_kevin_random copies the six-byte seed; main seeds it from the literal “kevin” (src/obj_seg000.c:500). update_gamestate snapshots include the seed and restore it with GAMESTATE (src/obj_seg001_complete.c:1707-1712, 1684-1692). Crash debris consumes this generator (src/obj_seg001_complete.c:3163-3168). Preserve exact PRNG state and call order.

The .RPL stores GAMEINFO + track map + one event byte per recorded frame, but the writer does not serialize the six-byte Kevin seed (src/obj_seg005.c:1280-1290). Main’s random_wait mixes rand() and get_kevinrandom() after a video-status wait (src/obj_seg031.c:256-267), and race setup consumes get_kevinrandom() as well (src/obj_seg005.c:606). In-memory snapshots make seeking within a running replay reproducible, but a saved replay alone does not carry the seed needed to guarantee cross-session reproduction of any random physics effects. Either preserve the legacy startup/call sequence or add a separately justified seed capture mechanism; do not claim the file is self-sufficient for random-state reproduction.

For a deterministic SDL port, the minimum tick boundary to preserve is: current GAMESTATE + Kevin seed + current event byte + track/car parameter tables -> the ordered update_gamestate call closure -> next GAMESTATE + PRNG seed. Rendering may run at a separate cadence only after the game-frame/timer coupling above is retained. Audio and camera/debris update calls are visibly inside this closure; moving or deleting them requires evidence that they do not alter authoritative state.

## Rendering, buffers, palette, and retrace

run_game chooses its drawing target before each frame. With g_videoflg5 nonzero it calls setup_mcgawnd2 and uses numid as page; otherwise it calls sprite_copy_wnd_to_1. It conditionally prepares the dashboard/replay bar and car shapes, calls update_frame, draws mask/cockpit/clip overlays, then, only in the nonzero flag path, calls setup_mcgawnd1 and toggles numid with XOR 1 (src/obj_seg005.c:718-815). g_videoflg5 is set to zero during initialization (src/obj_seg031.c:117); in the observed default path there is no per-frame numid flip. This is software-window buffering/page selection, not evidence of VGA hardware page flipping.

Within update_frame, the game builds transformed track/car shape entries, orders them by distance with heapsortorder, and submits them through trans_op; then it renders the skybox and polygon/background, crash explosion shapes, windscreen crack/sink state, elapsed-time text, and HUD text (src/obj_seg003.c:823-850, 1817-1964; image range seg003:0xA0F4-0xC302). run_game draws dashboard/replay-bar and car masks around that 3D call; overlays are not all part of the 3D sort queue (src/obj_seg005.c:770-804). Preserve the scene order, sort keys, and clipping interactions; the image-matched order is the porting contract.

setup_mcgawnd2 selects the offscreen sprite window as the drawing target; setup_mcgawnd1 selects sprite2 and blits that window to it (src/seg033_mcgawnd.c:12-25). Startup uploads the 256-entry palette as described earlier. No palette upload or explicit retrace wait occurs in the accepted per-frame run_game/update_frame source examined. A video-status service reads retrace/status bit 3 (video_get_status image seg012:0x22FFC-0x23006); initialize_main calls it from random_wait only. A separate helper at seg012:0x22FEE-0x22FFC spins until bit 3 changes, but context.py reported no callers. Consequently, do not insert an assumed wait-for-vsync at every SDL frame: retrace pacing remains an unverified platform behavior.

## Audio driver contract and timer hooks

The game loads a raw .DRV binary, treats its base as executable code, and calls far entry points by byte offset (src/obj_seg027.c:440-490). The first entry at offset 0 is called as unsigned char far func(void); its returned voice count 0 or 0xFF is rejected. Counts above 0x7F select driver_mode 1 and are capped to 16. The driver is reset and audiodriver_timer is registered after a valid load. Default code pc15 and /sSB -> ad are game-side selectors; do not presume the same hardware mapping in SDL. AD15.DRV's runtime return value, selected mode and effective audio-hook cadence were not measured; see [runtime measurements](runtime-measurements.md#explicitly-unmeasured).

The following table records only observed game-side call shapes, not undocumented vendor semantics. Typed pointer arguments shown are far pointers in the C call. Offsets are relative to the loaded driver base.

| Offset | Observed use and game-side call shape | Evidence |
|---|---|---|
| +0x00 | unsigned char far function(void); returns voice count | src/obj_seg027.c:461-475 |
| +0x03, +0x06 | no-argument entries called during teardown/reset | src/obj_seg027.c:495-512, 712-728 |
| +0x09 | event/sample start: (int, AUDIOVOICE*, AUDIOCHUNK*, int, int, char far*) | src/obj_seg028.c:436-445 |
| +0x0C, +0x0F | voice operations: (int, AUDIOVOICE*) | src/obj_seg028.c:496-500, 643-657 |
| +0x12 | program/type update: (int, AUDIOVOICE*, int), or (int, null, int) in channel mode | src/obj_seg028.c:200-215, 310-332 |
| +0x15 | event/controller update: (int, AUDIOVOICE*, int, int), or (int, null, int, int) in channel mode | src/obj_seg028.c:280-295 |
| +0x18 | no-argument init entry | src/obj_seg027.c:157-173, 712-728 |
| +0x1B | chunk parameter update: (AUDIOCHUNK*, int, int) | src/obj_seg028.c:298-308 |
| +0x1E | reset/stop channel: (int) | src/obj_seg027.c:712-728; src/obj_seg028.c:754-778 |
| +0x21 | load sample/resource: (int, AUDIOVOICE*, AUDIOCHUNK*, char far*); channel mode passes null for AUDIOVOICE* | src/obj_seg028.c:200-215, 335-362 |
| +0x24 | (int, AUDIOVOICE*, unsigned short) for event delta; (int, AUDIOVOICE*, int) for a value update | src/obj_seg028.c:436-460 |
| +0x27 | per-voice update: (unsigned int, AUDIOVOICE*, AUDIOCHUNK*, char far*) | src/obj_seg028.c:748-751 |
| +0x30 | flush/update voice array: (AUDIOVOICE*) | src/obj_seg027.c:194-200; src/obj_seg028.c:748-752 |
| +0x39 | opaque audio-event bytes: (int, unsigned char*) | src/obj_seg028.c:170-182 |
| +0x3F | command buffer: (int, void far*); game sends 4 bytes {0x10, 0, 0x16, volume} | src/obj_seg027.c:153, 389-417, 477-484 |
| +0x42 | MT-32 patch-bank pointer: (void far*) | src/obj_seg027.c:477-484 |

Shared game-side data includes 24 packed AUDIOCHUNK records of 0x4C bytes and 16 AUDIOVOICE records of 0x2E bytes (src/obj_seg027.c:15-62). Known AUDIOCHUNK members include far data pointers at offsets 0 and 5, index at 0x23, priority at 0x24, volume at 0x28, another far pointer at 0x2E, and a long at 0x48; known AUDIOVOICE members include state at 1, position at 8, length at 0x0C, and additional words/flags through 0x2D. These records are packed; fields still named unk* remain part of the interface because the code passes and/or updates those whole objects. Engine audio uses a separate 25-entry AudioTimer table of 0x4C-byte records; its 0x30-byte AudioPayload is embedded at offset 0x1C and contains eight resource pointers (src/obj_seg007.c:7-36, 97-145). Do not conflate these engine profiles with the general voice table.

The general driver timer checks loaded state, update lock, and reentry lock, then processes music or voice state and effect chunks (src/obj_seg028.c:110-127). The engine audio subsystem independently registers audio_driver_timer with the PIT callback table and removes it at race shutdown (src/obj_seg007.c:63-83; src/obj_seg005.c:885-910). Its callback smooths pitch/rate with integer moving averages, may suppress every other callback in one driver mode, and sends changed values to audio helpers (src/obj_seg007.c:180-227). audio_carstate is also called from update_gamestate each active tick (src/obj_seg001_complete.c:1726-1743).

These call sites establish far entry addresses, game-side argument layouts, and shared records. The game declares an unprototyped far DRVPROC at src/obj_seg027.c:119 and also casts entries to typed far function pointers. Exact callee stack cleanup rules and semantics of opaque resource/event records remain unresolved because the matching driver implementation is not in these sources. Keep the call signatures and record sizes; obtain the matching .DRV implementation or a verified runtime trace before assigning additional meanings.

## Port invariants and explicit unknowns

Preserve these observed contracts:

- Startup order and switches; load the same game assets and distinguish video mode, audio-driver code, and gameplay settings.
- One event byte per simulation step; collect device state at that boundary and preserve the separate temporary analog-steering path.
- Timer callback bookkeeping and 10/20 logical frames-per-second choices; keep hardware tick frequency marked conditional until measured.
- Exact GAMESTATE, fixed-point arithmetic, tables, PRNG state/call order, and checkpoint seek semantics.
- Track-map/replay byte extents, .HIG record length, and resource-loading order.
- 3D sorting and the 2D/dashboard/overlay order; preserve buffer-mode conditions and palette upload.
- Driver offsets, far entry calls, event records, and shared record extents without inventing vendor ABI meanings.

Open evidence gaps: actual PIT channel/rate under the target DOS/emulator setup; exact driver callee cleanup and undocumented audio record formats; literal editor button/dialog wording inside packed resources; and exact presentation/retrace behavior on the intended video hardware. The saved-replay seed omission is known from accepted serialization code, but makes cross-session random-effect reproduction dependent on startup and PRNG call order. These do not license substituting guessed timing, editor logic, or driver semantics.

