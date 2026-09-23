# Pristine oracle and coordinates

## Construction

Only `MCGA.HDR`, `EGA.CMN`, `MCGA.DIF`, `MCGA.COD` in the user's `assets/` supply target bytes. The supplied COD/CMN/DIF files are DSI 1.1 Huffman compressed. A simple canonical bit decoder reconstructs components of 143104, 23062 and 55509 bytes respectively. All three independently agree with Restunts' predecoded combiner inputs. Restunts files are never read by the oracle constructor.

The DIF decoder starts at offset -1, advances by each low-15-bit delta, and writes two bytes (or four when bit15 is set). A zero word terminates the stream. It rejects missing terminators, trailing bytes, truncation and out-of-range writes.

The packed executable consists of the 30 distributed header bytes, 482 deterministic zero padding bytes, patched common component and MCGA code component. The Restunts C++ combiner did not initialize those 482 bytes. Our packed file hash therefore identifies a canonical combined representation, not an unavailable original disk EXE with historically recoverable padding.

EXEPACK decoding recognizes the observed 18-byte RB header, skip_len=1, exact stub suffix and 16 relocation banks. It preserves the uncompressed prefix, processes backwards copy/fill commands, and reconstructs every relocation pair in encoded order. Unsupported variants fail closed. Packed outer relocations are rejected.

| Representation | Bytes | SHA-256 |
|---|---:|---|
| Canonical packed MZ | 199125 | `f57ac2775b6156b0413775eb61a1c48743f64248f72d13ca970dc645029eb41d` |
| Canonical unpacked MZ | 210384 | `1adb8259b3f6634062b94826e1f167649d9f2deeb6cedc9989127fc37e9ce615` |
| Pristine load image | 200000 | `4fc3340e2dc5d139a95a9a92278ef09902003d2c6857e5d9cdf0112870f3789e` |

There are 2588 relocations. Restored entry is `1CC5:0012`, stack `3649:1F40`. The unpacked header is 10384 bytes (`0x2890`). Entry/stack and relocation information survive packing; original header sizes, checksum, padding and prepacking allocation policy do not. Our unpacked header uses minimal paragraph alignment and a documented ceiling-based minalloc reconstruction.

Independent UNP 4.11 under hidden DOSBox reproduced all load bytes and the entire ordered relocation table. Its `e_minalloc` is one paragraph smaller than ours (0x769 versus 0x76A), reflecting different rounding. This is reconstructed metadata, not a code mismatch. UNP under MS-DOS Player returned zero despite an INT21 error; that failed run was not accepted. See `recovery/unp-crosscheck.json` and `recovery/unp-command.json`.

## Coordinate contract

`tools/coordinates.py` keeps these spaces explicit:

- Distributed offsets refer to compressed individual files. No global byte-offset delta into the program exists.
- Decoded component/DIF offsets are recorded in `build/oracle/transforms.json`.
- Packed-MZ offsets add 512 to packed load offsets. Only literal copy/prefix intervals have a unique direct unpacked mapping; fill commands are many-to-one provenance.
- Unpacked-MZ file offsets add `0x2890` to load-image offsets.
- Segment:offset uses `segment*16+offset`, with the segment supplied explicitly. Normalized aliases are not substituted for original relocation pairs.
- Restunts IDA addresses subtract `0x10000` for this pinned pristine-core correspondence.
- Driver-integrated addresses have no default conversion and are rejected.

The IDA/core transform is supported by 781 distributed 256-byte binary anchors, 5191 instruction intervals, and 25 segment frames proved by relocated far calls to mapped named functions. Comparing every byte against the pinned cracked reference finds only image offset `0x2AC`: pristine 0, cracked 1 (reference file offset `0x2B3C`). The driver-integrated executable is never used for this proof.

The importer obtains 234 full instruction-anchored procedure extents out of 619. A further containment function has its complete 52-byte compiler contribution proven through promotion. Symbolic memory displacements are not fully checked by the instruction importer, so imported semantics remain hypotheses even when boundaries are strong. Partial/unanchored procedures remain explicit and are not silently assigned neighboring boundaries.
