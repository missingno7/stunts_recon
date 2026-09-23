"""Strict EXEPACK decoder for the observed 18-byte RB header.

Format/algorithm cross-reference: viiri/exepack src/exepack.rs, see references
lock. No external executable or Restunts binary supplies any output bytes.
"""
import struct
from common import require, identity
from mz import MZ

def decompress(data, size):
    require(0 < size <= 0x100000, 'Invalid unpacked size')
    out = bytearray(data + bytes(max(0, size - len(data))))
    src, dst = len(data), size
    for _ in range(15):
        if src and out[src - 1] == 255:
            src -= 1
        else:
            break
    commands = []
    while True:
        require(src >= 3, 'Truncated EXEPACK command')
        command = out[src - 1]
        count = struct.unpack_from('<H', out, src - 3)[0]
        src -= 3
        require(dst >= count, 'EXEPACK output underflow')
        dst -= count
        if command & 254 == 0xB0:
            require(src >= 1, 'Truncated EXEPACK fill')
            src -= 1
            out[dst:dst + count] = bytes([out[src]]) * count
            kind = 'fill'
        elif command & 254 == 0xB2:
            require(src >= count, 'Truncated EXEPACK copy')
            src -= count
            for i in range(count - 1, -1, -1):
                out[dst + i] = out[src + i]
            kind = 'copy'
        else:
            raise ValueError(f'Unknown EXEPACK command {command:02x}')
        commands.append({'kind': kind, 'packed_load_start': src, 'load_start': dst, 'length': count})
        if command & 1:
            break
    require(dst <= len(data), 'EXEPACK leaves uninitialized gap')
    commands.append({'kind': 'unchanged_prefix', 'packed_load_start': 0, 'load_start': 0, 'length': dst})
    return bytes(out[:size]), commands

def unpack(packed):
    mz = MZ.parse(packed)
    require(not mz.relocations and mz.ip == 18, 'Unsupported EXEPACK outer header')
    body = mz.load_image(packed)
    at = mz.cs * 16
    require(at + 18 <= len(body), 'EXEPACK header outside image')
    ip, cs, scratch, block_size, sp, ss, dest, skip, signature = struct.unpack_from('<9H', body, at)
    require(signature == 0x4252 and skip == 1, 'Unsupported EXEPACK signature/skip length')
    require(at + block_size == len(body), 'EXEPACK block does not end at MZ end')
    suffix = b'\xcd\x21\xb8\xff\x4c\xcd\x21Packed file is corrupt'
    # Stubs contain a 22-byte error string: the trailing LF is not part of it.
    block = body[at:at + block_size]
    require(block.count(suffix) == 1, 'Unknown/ambiguous EXEPACK stub suffix')
    reloc_at = block.index(suffix) + len(suffix)
    stub_id = identity(block[18:reloc_at])
    table_start = reloc_at
    relocs = []
    for bank in range(16):
        require(reloc_at + 2 <= len(block), 'Truncated relocation count')
        count = struct.unpack_from('<H', block, reloc_at)[0]
        reloc_at += 2
        require(reloc_at + count * 2 <= len(block), 'Truncated relocation entries')
        for i in range(count):
            offset = struct.unpack_from('<H', block, reloc_at + i * 2)[0]
            require(bank * 65536 + offset + 2 <= dest * 16, 'Relocation outside unpacked image')
            relocs.append({'segment': bank * 4096, 'offset': offset, 'load_offset': bank * 65536 + offset})
        reloc_at += count * 2
    require(reloc_at == len(block), 'Unexpected trailing relocation data')
    require(len({r['load_offset'] for r in relocs}) == len(relocs), 'Duplicate relocations')
    image, commands = decompress(body[:at], dest * 16)
    header_size = (28 + 4 * len(relocs) + 15) // 16 * 16
    size = header_size + len(image)
    minimum = (len(body) + 15) // 16 + mz.minalloc - len(image) // 16
    require(0 <= minimum <= 65535, 'Invalid reconstructed minalloc')
    header = struct.pack('<14H', 0x5A4D, size % 512, (size + 511) // 512,
                         len(relocs), header_size // 16, minimum, mz.maxalloc,
                         ss, sp, 0, ip, cs, 28, 0)
    header += b''.join(struct.pack('<HH', r['offset'], r['segment']) for r in relocs)
    header += bytes(header_size - len(header))
    result = header + image
    MZ.parse(result)
    return result, {'header': {'ip': ip, 'cs': cs, 'ss': ss, 'sp': sp, 'dest_paragraphs': dest,
                              'skip': skip, 'block_size': block_size, 'scratch': scratch},
                    'stub': stub_id, 'packed_relocation_block_offset': table_start,
                    'relocations': relocs, 'commands': commands}
