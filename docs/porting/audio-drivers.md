# Stunts 1.1 audio drivers and bank formats

**Scope.** Static interoperability reference for the four original raw driver images and the audio bank assets currently present under `assets/`. Driver offsets below are byte offsets from the loaded `.DRV` base. Game image addresses are identified separately by `segNNN:`.

## Loading and call boundary

`audio_load_driver` in `src/obj_seg027.c:440-490` loads the `.DRV` as a raw binary and calls `base+0` as a far function returning `unsigned char`. The images start with relative near-jump stubs; none starts with an MZ header. `base+0` returns 0 or FFh on failure. A return above 7Fh selects `audio_driver_mode=1` and the game fixes the usable voice/channel count at 16; lower values are used as the voice count. The game calls `base+18` to initialize, `base+1E` to reset channels, and `base+06` then `base+03` when unloading.

The game-side code uses **direct far calls into the loaded image**, not INT 60h/61h/62h: the cast call sites in `src/obj_seg027.c` and `src/obj_seg028.c` name offsets such as `+09`, `+21`, `+39`, `+3F`, and `+42`. No reachable `INT 60h`, `INT 61h`, or `INT 62h` instruction was found in these four images. TD15 does use DOS INT 21h to save/install/restore INT 15h and BIOS INT 1Ah timer services.

In the medium-model caller, unqualified `AUDIOVOICE *`, `AUDIOCHUNK *`, and `unsigned char *` arguments are **near** DS pointers; only arguments explicitly declared `far` take offset:segment stack slots. The services below are far-call entries and end through far returns. Argument zero begins at `[BP+6]`; subsequent argument positions depend on near (2-byte) versus far (4-byte) pointers.

### Public call shapes established by game callers

| Offset | Game-side call shape | Use / evidence |
|---|---|---|
| `+00` | `unsigned char far (void)` | Detect/initialize device and report voice/channel count; `obj_seg027.c:461-475`. |
| `+03`, `+06` | `void far (void)` | Teardown/reset lifecycle calls; `obj_seg027.c:495-512, 712-728`. |
| `+09` | `(int, AUDIOVOICE *, AUDIOCHUNK *, int, int, char far *)` | Start sample/note event; `obj_seg028.c:436-445`. |
| `+0C`, `+0F` | `(int, AUDIOVOICE *)` | Stop/release and force-off voice paths; `obj_seg028.c:496-500, 643-657`. |
| `+12` | `(int, AUDIOVOICE *, int)`; voice may be null in channel mode | Program/type update; `obj_seg028.c:200-215, 310-332`. |
| `+15` | `(int, AUDIOVOICE *, int, int)` | Event/controller update; `obj_seg028.c:280-295`. |
| `+18` | `void far (void)` | Driver init/reset hook; `obj_seg027.c:157-173, 712-728`. |
| `+1B` | `(AUDIOCHUNK *, int, int)` | Chunk parameter update; `obj_seg028.c:298-308`. |
| `+1E` | `(int)` | Reset/stop channel; `obj_seg027.c:712-728`, `obj_seg028.c:754-778`. |
| `+21` | `(int, AUDIOVOICE *, AUDIOCHUNK *, char far *)`; voice may be null in channel mode | Bind/load sample or instrument resource; `obj_seg028.c:200-215, 335-362`. |
| `+24` | `(int, AUDIOVOICE *, unsigned short)` or `(int, AUDIOVOICE *, int)` | Event delta/value update; `obj_seg028.c:436-460`. |
| `+27` | `(unsigned int, AUDIOVOICE *, AUDIOCHUNK *, char far *)` | Per-voice update; `obj_seg028.c:748-751`. |
| `+30` | `(AUDIOVOICE *)` | Bulk voice-array update, used in pause path; `obj_seg027.c:194-200`. |
| `+39` | `(int, unsigned char *)` (near byte buffer) | Forward opaque bank bytes; `obj_seg028.c:170-182`. MT15 sends them to its MIDI output; the other three implementations are inert. |
| `+3F` | MT15 only: `(int, void far *)` | Send command bytes; game passes four-byte `{10h,0,16h,volume}` buffer; `obj_seg027.c:153,389-417,477-484`. |
| `+42` | MT15 only: `(void far *)` | Load the optional `mt32.plb` patch bank; `obj_seg027.c:477-484`. |

The offsets `+2A`, `+2D`, `+33`, `+36`, and `+3C` are present in the common jump table, but no game caller was found for them. Their raw stack accesses are recorded below; a semantic/API prototype is not inferred from callee implementation alone.

Static-only slot notes (unresolved public meanings; **BLOCKER CLASS 8**):

- `+2A`: AD15, PC15, and TD15 use a word at `[BP+0A]` and a pointer-like value at `[BP+8]`, writing the word at pointer offset `+4`; PC15 also writes it at `+6`, and TD15 additionally tests `[BP+6]`. MT15 is a no-op. The caller-visible aggregate/field meaning is unknown.
- `+2D`: AD15/PC15 read byte/word/byte from `[BP+8]`, `[BP+0A]`, `[BP+0C]`; TD15 branches on a word at `[BP+6]` and then uses further scalar inputs; MT15 is a no-op. Exact logical prototype and purpose are unknown.
- `+33`: AD15 takes a far resource pointer at `[BP+8]` and scalar inputs at `[BP+0C/+0E]`, validates resource bytes and copies a pointed-to record; PC15/TD15 use an integer at `[BP+6]` to control timer/interrupt state; MT15 is a no-op. There is no located caller to identify valid values.
- `+36`: AD15 tests an integer at `[BP+6]`; PC15 takes an integer and returns a status-like AX value; TD15 invokes BIOS INT 1Ah AH=81h and returns its result; MT15 returns `00FFh`. Shared logical meaning is unknown.
- `+3C`: zero-argument status/read slot; MT15 pops one byte from its ring buffer or returns `FFFFh` when empty, other drivers return `FFFFh`. No caller was located.

### Jump-vector targets

All common slots are three-byte near-jump stubs. This table gives each stub's resolved target (hex, file-relative). MT15 alone continues the table at `+3F/+42`.

| Slot | AD15 | MT15 | PC15 | TD15 |
|---:|---:|---:|---:|---:|
| `+00` | `0A7E` | `0400` | `02F2` | `0579` |
| `+03` | `0ADA` | `0423` | `031E` | `05C2` |
| `+06` | `005B` | `0428` | `0326` | `05D4` |
| `+09` | `010B` | `0451` | `033F` | `061A` |
| `+0C` | `01EF` | `048C` | `03B3` | `06D7` |
| `+0F` | `0213` | `04A7` | `03DA` | `071C` |
| `+12` | `03C0` | `04AC` | `03B8` | `06DC` |
| `+15` | `0250` | `04C7` | `03CA` | `06F4` |
| `+18` | `04F2` | `04E0` | `03CF` | `0711` |
| `+1B` | `04F2` | `04FB` | `03D5` | `0717` |
| `+1E` | `00F5` | `0543` | `03DA` | `071C` |
| `+21` | `006B` | `055A` | `03F4` | `0765` |
| `+24` | `04A8` | `051E` | `03F9` | `0784` |
| `+27` | `02C6` | `05D7` | `0423` | `07E5` |
| `+2A` | `0449` | `06BA` | `0537` | `0A06` |
| `+2D` | `03E9` | `06BF` | `0548` | `0A23` |
| `+30` | `04F2` | `06C4` | `05DF` | `0AE2` |
| `+33` | `0457` | `06C9` | `0791` | `0B33` |
| `+36` | `0495` | `06CE` | `089B` | `0BA6` |
| `+39` | `04F2` | `03ED` | `02ED` | `0574` |
| `+3C` | `04F2` | `02FA` | `02E9` | `0570` |
| `+3F` | no slot | `0321` | no slot | no slot |
| `+42` | no slot | `032F` | no slot | no slot |

AD15 has ordinary code beginning at `+3F`, but it is not a far-call stub: that internal routine ends with a near `RET` at `+5A`. PC15/TD15 have no extension vector there. Do not treat the bytes after `+3C` as common ABI slots.

## Driver backends and differences

### AD15.DRV — AdLib/OPL path

At `+00 -> 0A7E`, the initializer probes base candidates `0388h`, `0318h`, `0288h`, then `0218h`, storing the selected base at driver CS:`0972`; failure returns 0 and the success path returns `0Ah` (`0A7E-0AD9`). Its I/O helper at `0806-0810` reads status at the selected base; the register writer at `07EA-07F8` writes to the base and base+1, consistent with an OPL register/data pair. The driver advertises ten logical slots: channel 0 is the separate sample/INT 8 path, while channels 1–9 map to OPL channels 0–8 using the interleaved operator-offset table at CS:`0870`. The `+03/+06` routines reset nine OPL channels (`0ADA-0AE9`, `005B-006A`). Every record in the supplied AD VCE banks has zero at `+0A`, so those records do not enter the channel-0 sample path.

The SDL host now selects AD15 by default and models the observed OPL register calls in `port/ad15_driver.c`; it uses the bundled driver's lookup tables as inert data and never executes its instructions. The register interface is rendered by the pinned Nuked OPL core through SDL's audio stream. The focused regression `tests/sdl3/ad15_driver_probe.c` compares initialization, instrument setup, note, volume, reset, parameter, and pitch-bend register writes against calls to the immutable AD15 image in the isolated Unicorn oracle at `build/workers/ad15_oracle/ad15_oracle.py` / `.json`. A separate OPL wrapper regression compares PCM against an independently initialized instance of the same pinned upstream core. These checks cover the translated melodic path; AD15 channel-zero sample playback remains unsupported. The shipped AD VCE records have zero at `+0A`, the condition the driver tests before entering its sample path. MT15 and TD15 remain unsupported host backends.

### MT15.DRV — MPU-401 / MT-32 path

At `+00 -> 0400`, startup sends `FFh`, waits, sends `3Fh`, resets channel state, and returns `AX=FFF6h` (`0400-0422`). The game reads low byte `F6h`, selects mode 1, and exposes 16 MIDI channels. The send/status helper polls port `0331h` and transfers data at `0330h` (`01C7-020A`, `0233-025E`). `+39 -> 03ED` sends the given near buffer byte-by-byte through that helper. `+3F -> 0321` forwards the caller's far buffer and length to the driver's serial-send path; `+42 -> 032F` parses/sends the supplied patch-bank bytes. The game attempts to load `mt32.plb` before calling `+42`; that file is not among the audio assets inventoried here.

The `GEENG.SFX/MTIN` track contains 19 `E8` raw-byte events; all payloads begin `F0 41` (Roland manufacturer SysEx data) and the final stored byte is `00h`. In this track every delta is one byte, so `AudioEvent.length-4` passed to `+39` sends payload length minus one and drops that trailing zero. The parser reports stored payload and actual driver-send length separately. `+3C -> 02FA` drains one byte from the driver's 40-byte receive ring and returns `FFFFh` when empty; the game currently has no caller for it.

### PC15.DRV — PC speaker / PIT path

At `+00 -> 02F2`, PC15 writes control `B6h` to PIT control port `43h`, clears its state and returns 7 logical voices (`02F2-031D`). `+06 -> 0326` clears speaker gate bits at port `61h`; note synthesis updates PIT channel 2 through `42h` and the speaker gate through `61h` (notably `0681-069B`, `0847-0851`). Seven is the software driver's reported slot count; the observed physical output is one PIT/speaker path, not seven independent hardware oscillators. PC15 `+30 -> 05DF` is a real bulk-voice routine; its common-table `+33/+36` slots access timer/interrupt state but lack a game-side caller.

The SDL host model translates PC15's channel-zero sample path from `+09 -> 033F`, its private helper at `0791`, and timer handler at `0701`. The helper reads a nested far pointer from the supplied voice record at `+6`, uses the low word of the four-byte field at descriptor `+8` as the stream length, and begins sample bytes at `+32h`. It sets PIT0 reload to 70 (the quotient `001234DCh / 4268h`), computes a 16.16 cursor step as `rate / 17000`, maps each byte through the 5..70 count table, and retriggers PIT2 mode 1 while toggling PPI gate bit 0. The handler stops after its one pass and restores PIT0/PIT2 state through `06A8`. The SDL renderer advances those modeled PIT2/PPI states against the PIT clock and samples them at 48 kHz; it never installs a guest interrupt vector or executes the raw driver image. `audio_stop_unknown` releases only a host-owned stream timer, matching the original routine's vector-ownership guard at the platform boundary.

The sample helper has a contained original-machine comparison. `build/workers/audio/pc15_unicorn_probe.py` calls the immutable `assets/PC15.DRV` only in Unicorn with an in-range synthetic far descriptor, then invokes its original INT 8 body. For a 64-byte stream at rate-table note 35, the driver makes 138 IRQs, writes 137 sample reloads, hashes them to `0x110acd13`, and restores the old INT 8 vector. `tests/sdl3/pc15_audio_probe.c` checks the host cursor, byte count, reload hash, and mode-1 output against that trace. The probe is a scratch oracle; the production SDL build does not load or execute driver instructions.

PC15's mode-3 speaker path preserves the 8253's count-latching rule. The original `+30` routine rewrites PIT2's LSB/MSB pair while the square wave is running. Per the Intel 8253 datasheet, a mode-3 count reload takes effect immediately after the current output transition, so it must not reset the half-cycle at the software write; see the [8253 datasheet, mode 3](https://sapr.asvcorp.ru/datasheets/64/05/00000000564.pdf). `port/pc_speaker.c` retains a pending count and applies it at the next half-cycle edge. The focused regression replays the live engine's 100 Hz writes of `65417`, `65298`, and `0` and requires a bipolar PCM signal; resetting phase on each two-byte write left this low-frequency engine voice stuck at +4096. Against the latest SDL3 executable, six live callback blocks (6,144 frames) for the same channel-2 engine sample contain both `+4096` and `-4096` with five transitions.

The exact seeded driving run also reaches the real engine-bank path: the loaded `eng1` profile supplies PCENG1.VCE's `ENGI` record at file offset `0x56`, whose voice mask `0x0014` selects channel 2. A contained original `+27` call on the captured pre-call voice/chunk/record snapshot returns pitch word `0xFF89`, matching the translated host update. The live engine sends `+24` values below PC15's `0x0130` threshold, followed by the per-voice `+27` update and `+30` PIT2 write. The run does not dispatch channel 0; its live engine event uses channel 2, and PCENG1's `CRA2` channel-zero record is not selected by the tested `GEENG.SFX` engine/effect path.

The sample path is conditional on a valid host-rebased pointer. The supplied PC VCE records do not by themselves provide one: PCSKIDMS sample records have zero at `+6`, and PCENG1's `CRA2` record contains the literal `cras` name there. The code validates the pointer and complete descriptor span against managed host allocations, then leaves unresolved spans explicitly unsupported and silent. The source helper `link_audio_shape_resources` has no caller in the checked C source, so this port does not fabricate a resource resolution step.

The host supports the observed PC15 channel 1–6 tone/mix path and the validated synthetic channel-zero stream path. Channel-zero playback from the bundled PC VCE banks remains unsupported until the loader can provide the far sample descriptor that the original helper expects. AD15's OPL melodic path is implemented as described above; MT15 and TD15 remain unsupported host backends, so their MIDI and Tandy output paths are not rendered.

### TD15.DRV — Tandy sound path

At `+00 -> 0579`, TD15 initializes PIT channel 2, silences the speaker, initializes six channel levels, saves INT 15h, installs its own INT 15h handler, and returns 6 (`0579-05C1`). `+03 -> 05C2` restores the previous INT 15h vector; `+06 -> 05D4` gates the speaker and resets timer state. Sound writes go to `C0h/C1h` and `C6h/C7h` (`0548-056C`, `099A-09AA`), matching the Tandy PSG/DAC register path. Timer support uses BIOS INT 1Ah AH=83h to schedule/cancel and AH=81h to poll (`05E4-05F8`, `0B33-0BB0`). The driver reports six logical voices.

## Game-side state and scheduling

`src/obj_seg027.c:15-62` defines a packed 24-entry `AUDIOCHUNK` table (`0x4C` bytes each) and 16-entry `AUDIOVOICE` table (`0x2E` bytes each). `src/obj_seg028.c:1-45` overlays their playback fields. Known chunk offsets are: playback cursor `+00` (far pointer), nesting depth `+04`, four-entry return stack `+05`, active/max voices `+15/+16`, delay `+18`, selected resource/sample pointer `+1E`, velocity/resource id/note/mode `+22..+25`, program `+28`, sample-pointer table `+2E`, loop depth `+32`, loop positions `+33`, loop counts `+43`, channel `+47`, callback `+48`. Preserve the complete packed extent and uninterpreted members.

Known voice offsets are: resource index/active/note `+00/+01/+02`, position `+08`, remaining event duration `+0C`, sample far pointer `+10`, sample-rate/envelope accumulator `+14`, envelope state `+16`, loop/level scratch `+18..+29`, owner chunk pointer `+2A`, hardware channel `+2C`. The game owns the active-voice selection, duration, loop, and envelope progression; the driver services receive these packed records and perform chip-specific writes.

The separate engine layer in `src/obj_seg007.c:7-36,97-145` uses 25 `AudioTimer` records (`0x4C` each) with a 0x30-byte engine profile at offset `+1C`; the profile includes a shape pointer and eight resource-name pointers. `setup_player_cars` loads resources `"eng1"` and `"eng"` and passes player/opponent profiles to `audio_init_engine` (`src/obj_seg005.c:1619-1660`). Engine sound resources are selected through these profiles and the general driver chunk allocator, not through the KMS song timeline. `audio_op_unk2` updates a distance-attenuated target volume and pitch/rate from RPM and relative position (`src/obj_seg007.c:228-258`); the timer smooths targets and retriggers channels (`180-227,272-365`).

The general driver callback is registered at driver load (`src/obj_seg027.c:476`) and calls `audiodriver_timer` on timer callbacks (`src/obj_seg028.c:110-127`). Music processing adds `0x80` to `snd_sample_rate_phase` and processes as many logical music steps as `mus_samplelimit` allows; default limit is `0x80`, while event `DD` sets `32000/parameter` (`src/obj_seg027.c:159-173`, `src/obj_seg028.c:129-140,201-206`). Effects are scanned every driver callback (`obj_seg028.c:142-149`, chunks `0x10..0x16`). Engine audio registers a second callback (`obj_seg007.c:63-83`) and, in mode 1, updates on every other callback (`obj_seg007.c:180-227`).

The exact physical timer frequency remains conditional. `asm/timer_setup_interrupt.ASM:7-13,41-76` loads divisor `0x2E9C` and writes PIT control `0xB6` to port `43h`, then writes the divisor to port `40h`; the control byte selects channel 2 while `40h` is channel 0 data. The project's runtime note therefore does not establish 100 Hz. Preserve the software callback cadence and event phase rules; measure absolute frequency under the target DOS/emulator configuration before choosing an SDL clock.

## Bank formats

The read-only reference parser is [`tools/porting/audio_bank.py`](../../tools/porting/audio_bank.py). It uses only Python's standard library and reads `assets/` without modifying it. Run `python tools/porting/audio_bank.py --verify-all --json build/porting/audio-bank-validation.json`. On the current asset set it validates all 13 audio bank files, all 105 VCE records, 13 nested KMS/SFX song archives, and 4,802 events. Full parsed values and source byte offsets are preserved in the JSON output under `build/`.

### Common resource directory

The game implementation at `src/obj_seg029.c:41-61` establishes this directory layout:

~~~
u32 declared_size
u16 chunk_count
char names[chunk_count][4]       // case-insensitive FourCC lookup
u32 offsets[chunk_count]         // little-endian, relative to data_base
byte data[]
data_base = archive_start + 6 + 8*chunk_count
chunk_i = data_base + offsets[i]
~~~

The first `u32` is used as an archive/chunk extent in KMS/SFX and agrees with their indexed bounds. VCE length words are advisory: in MTSKIDMS.VCE only, the final 0x5D-byte SNTH record crosses the nominal end by six bytes and remains fully present in the raw file. The parser uses the physical VCE file bound, validates each indexed record, and reports trailing bytes rather than truncating them. Directory order is not data order; resolve each chunk by its offset.

`.KMS` has one outer named chunk (`over`, `slct`, `titl`, or `vict`) whose payload is another resource directory. `.SFX` in this asset set (`GEENG.SFX`) has nine outer FourCC entries, each pointing to a separate inner directory. Inner music/effect directories contain `HDR1` plus named track chunks; the header maps instrument FourCC names and track FourCC names. The game patches those names into far pointers in memory (`audio_map_song_instruments` and `audio_map_song_tracks`, `src/obj_seg027.c:644-804`); the file itself keeps the names and relative offsets.

VCE entries are driver-specific voice/preset records rather than song tracks. Their first word is record size and the next word is revision 1. AD15 records are 0x64 bytes; MT15, PC15, and TD15 records are 0x5D bytes. Retain each complete indexed record in `raw_record`, including unmapped gaps and the tail. Seven of the eight supplied VCE archive size words differ from physical file size; validate records against physical bounds and do not truncate at the advisory header size. The following offsets are mapped reads, not a complete schema.

Game-side accesses (src/obj_seg028.c:30-64,372-373,410,436-484) include: +05 sample-kind selector (value 5 routes percussion names), +0C voice mask, +10 event-note bias, +1C a word copied into AudioVoice.sampleRate, +2A/+2C/+30/+34/+36/+38 words/byte copied into initial voice state, and +43 explicit channel override (values below 16; otherwise a channel is derived). The existing parser also exposes the layout’s envelope, loop, limit, flag and pulse fields at +1E..+3B; bytes not directly interpreted by the game remain raw.

Static reads from the supplied driver entry points show that the record tail is not all reserved. AD15 +09 (file offset 0x010B) reads record +0A, +11 and follows the far slot at +06; AD15 +21 (0x006B) reads +16..+18 and +44/+45. MT15 +09 (0x0451) tests +15; MT15 +21 (0x055A) stores +12 and emits +44 as MIDI program change, then +45 as controller 7 when nonzero and +46 as controller 10. TD15 +09 (0x061A) tests +15; TD15 +21 (0x0765) forwards +28 as parameter 7. The corresponding PC15 entries +09 (0x033F) and +21 (0x03F4) do not dereference the supplied record pointer in those entry bodies. These are positive offset-use observations from the four assets/*15.DRV images; other driver code may read additional bytes. Unmapped gaps and every complete raw record must still be preserved.

### Song/effect event stream

Each non-`HDR1` track begins with a 32-bit total span, including that size word; event bytes occupy `[track+4, track+span)`. The `GEENG.SFX` and KMS indexed spans equal these track spans in every parsed asset. Each event is a base-128 continuation delta followed by one command. This matches `read_audio_event` in `src/obj_seg028.c:544-593`.

| Command | Encoded following bytes | Observed game behavior |
|---|---|---|
| `< D9h` | If command `>80h`, one parameter byte; then base-128 value | Channel/note event; values under `80h` inherit current velocity. Value is used as duration/voice event value. |
| `D9h`, `DAh`, `DBh`, `E3h` | none | Return from subtrack, stop, restart, loop end. |
| `DCh`, `DDh`, `DEh`, `E0h`, `E1h`, `E2h`, `E4h`, `E9h`, `EAh` | one byte | Select resource/program, tempo limit, voice event, max voices, note, loop begin, velocity, channel, audio value. |
| `DFh` | one byte parameter + one byte value | Voice parameter event. |
| `E5h` | little-endian word | Word-valued voice parameter. |
| `E6h` | one byte parameter + four-byte target | Call subtrack. The file's four-byte target is a chunk FourCC; game patch-up replaces it with a far pointer. |
| `E7h` | one-byte length + payload | Text/meta data. |
| `E8h` | one-byte length + payload | Raw driver bytes; copied to a temporary buffer and sent to driver `+39` with count `AudioEvent.length-4`. MTIN payloads have one-byte deltas and a trailing zero that the call drops. |

There are no other event commands in the parsed assets. For a replacement, keep bank FourCC lookup, instrument/track maps, delta accumulation, duration/envelope rules, and the `E6` loop/subtrack stack separate from chip synthesis. Do not interpret unclassified VCE fields as PCM: the observed game paths use voice parameter records to drive OPL, MIDI, PIT/speaker, or Tandy-register output.

## Evidence, confidence, and remaining limits

Driver loader and call signatures: `src/obj_seg027.c:440-512`; channel/resource calls: `src/obj_seg028.c:110-210,280-362,436-460,496-570,643-778`; archive lookup: `src/obj_seg029.c:41-61`; event decoder: `src/obj_seg028.c:544-593`; engine profile/timer: `src/obj_seg007.c:7-36,63-83,97-145,180-227,228-258` and `src/obj_seg005.c:1619-1660`. Driver vector and hardware anchors: AD15 `+00=0A7E`, `+09=010B`, OPL base helper `0806-0810`; MT15 `+00=0400`, MIDI I/O helpers `01C7-020A,0233-025E`, `+39=03ED`, `+3F=0321`, `+42=032F`; PC15 `+00=02F2`, `+06=0326`, speaker writes `0681-069B`; TD15 `+00=0579`, `+03=05C2`, timer/vector code `05A1-05D3`, Tandy writes `0548-056C,099A-09AA`.

The parsed bank layouts and event sizes are directly cross-checked against game-side reader code. Chip-specific driver semantics are strongest where an entry's instruction stream writes the hardware ports above; exact envelope mapping to each chip register, MT32 patch-bank (`mt32.plb`) internals, and absolute timer rate need a device/runtime trace or external device specification. The `+2A/+2D/+33/+36` slots' caller-visible meanings remain unknown even though their target bytes and raw frame accesses are catalogued in the scratch disassemblies.
