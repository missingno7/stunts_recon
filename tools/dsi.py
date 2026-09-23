"""DSI 1.1 canonical Huffman resource decoding, restricted to MCGA inputs.

Format evidence: Restunts fileio.c:file_decomp_vle and dstien/stunpack
dsi_huff.c (see layout/references.json). This deliberately uses a simple
bitwise canonical decoder instead of the original optimized prefix tables.
RLE/multipass and Stunts 1.0 are refused, not guessed.
"""
from common import require

def decode(data):
    require(len(data) >= 6 and data[0] == 2, 'Expected single-pass DSI 1.1 Huffman input')
    size = int.from_bytes(data[1:4], 'little')
    levels, additive = data[4] & 127, bool(data[4] & 128)
    require(0 < levels <= 16 and 0 < size <= 0x100000, 'Invalid DSI lengths')
    counts = data[5:5 + levels]
    require(len(counts) == levels and 0 < sum(counts) <= 256, 'Invalid Huffman alphabet')
    start = 5 + levels
    alphabet = data[start:start + sum(counts)]
    require(len(alphabet) == sum(counts), 'Truncated Huffman alphabet')
    tables, code, index = {}, 0, 0
    for width, count in enumerate(counts, 1):
        require(code + count <= 1 << width, 'Oversubscribed Huffman tree')
        for j in range(count):
            tables[width, code + j] = alphabet[index + j]
        index += count
        code = (code + count) << 1
    stream = data[start + len(alphabet):]
    bit, previous, out = 0, 0, bytearray()
    for _ in range(size):
        code = 0
        for width in range(1, levels + 1):
            require(bit < len(stream) * 8, 'Truncated Huffman bitstream')
            code = (code << 1) | ((stream[bit // 8] >> (7 - bit % 8)) & 1)
            bit += 1
            value = tables.get((width, code))
            if value is not None:
                previous = (previous + value) & 255 if additive else value
                out.append(previous)
                break
        else:
            raise ValueError('Invalid Huffman code')
    require(len(stream) * 8 - bit < 16, 'Unexpected trailing Huffman data')
    return bytes(out)
