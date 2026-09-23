"""All file/load-module coordinate conversions for the reconstruction."""
from dataclasses import dataclass
import struct


HEADER_FIELDS = (
    'e_magic', 'e_cblp', 'e_cp', 'e_crlc', 'e_cparhdr', 'e_minalloc',
    'e_maxalloc', 'e_ss', 'e_sp', 'e_csum', 'e_ip', 'e_cs', 'e_lfarlc', 'e_ovno',
)


@dataclass
class MZ:
    header_size: int
    declared_size: int
    relocation_offset: int
    relocations: list
    cs: int
    ip: int
    ss: int
    sp: int
    minalloc: int
    maxalloc: int

    @classmethod
    def parse(cls, data):
        return cls.parse_header(data, len(data))

    @classmethod
    def parse_header(cls, data, file_size):
        """Parse complete header bytes without needing any load-image bytes."""
        if len(data) < 28 or data[:2] != b'MZ':
            raise ValueError('Not a complete MZ header')
        _, last, pages, count, paragraphs, minimum, maximum, ss, sp, _, ip, cs, table, _ = struct.unpack_from('<14H', data)
        size = (pages - 1) * 512 + (last or 512)
        header = paragraphs * 16
        if not pages or last > 511 or not 28 <= header <= size <= file_size:
            raise ValueError('Invalid MZ header/load-image size')
        if len(data) < header:
            raise ValueError('Truncated MZ header')
        if table < 28 or table + count * 4 > header:
            raise ValueError('MZ relocation table outside header')
        relocations = []
        for i in range(count):
            offset, segment = struct.unpack_from('<HH', data, table + i * 4)
            target = cls.linear(segment, offset)
            if target + 2 > size - header:
                raise ValueError('MZ relocation target outside load image')
            relocations.append({'segment': segment, 'offset': offset, 'load_offset': target})
        return cls(header, size, table, relocations, cs, ip, ss, sp, minimum, maximum)

    @staticmethod
    def linear(segment, offset):
        return segment * 16 + offset

    def file_offset(self, load_offset):
        return self.header_size + load_offset

    def load_offset(self, file_offset):
        if not self.header_size <= file_offset < self.declared_size:
            raise ValueError('File offset is outside the DOS load image')
        return file_offset - self.header_size

    def load_image(self, data):
        return data[self.header_size:self.declared_size]

    def relocation_bytes(self, data):
        return data[self.relocation_offset:self.relocation_offset + 4 * len(self.relocations)]


def header_document(data):
    """One-time lossless decoding, used by the explicit recovery command only."""
    mz = MZ.parse(data)
    fields = dict(zip(HEADER_FIELDS, struct.unpack_from('<14H', data)))
    end = mz.relocation_offset + len(mz.relocations) * 4
    return {
        'format': 'empires-mz-header-v1',
        'fields': fields,
        # Keep original segment:offset pairs and ordering, not just linear targets.
        'relocations': [{'segment': r['segment'], 'offset': r['offset']} for r in mz.relocations],
        'before_relocations_hex': data[28:mz.relocation_offset].hex(' '),
        'after_relocations_hex': data[end:mz.header_size].hex(' '),
    }


def encode_header(document, file_size):
    """Build solely from structured source, preserving counts, order and padding.

    No input executable, default fill bytes, sorting, segment normalization or
    automatically corrected header fields are used by this encoder.
    """
    keys = {'format', 'fields', 'relocations', 'before_relocations_hex', 'after_relocations_hex'}
    if not isinstance(document, dict) or set(document) != keys or document['format'] != 'empires-mz-header-v1':
        raise ValueError('Invalid structured MZ header format or keys')
    fields = document['fields']
    if not isinstance(fields, dict) or set(fields) != set(HEADER_FIELDS):
        raise ValueError('MZ fields must contain exactly the 14 DOS header words')
    def word(value, label):
        if type(value) is not int or not 0 <= value <= 0xFFFF:
            raise ValueError(f'{label}: expected an unsigned 16-bit integer')
        return value
    header = struct.pack('<14H', *(word(fields[name], name) for name in HEADER_FIELDS))
    relocations = document['relocations']
    if not isinstance(relocations, list) or len(relocations) != fields['e_crlc']:
        raise ValueError('MZ relocation count does not match e_crlc')
    table = bytearray()
    for index, relocation in enumerate(relocations):
        if not isinstance(relocation, dict) or set(relocation) != {'segment', 'offset'}:
            raise ValueError(f'MZ relocation {index}: expected segment and offset')
        table.extend(struct.pack('<HH', word(relocation['offset'], f'relocation {index} offset'),
                                 word(relocation['segment'], f'relocation {index} segment')))
    gaps = []
    for key in ('before_relocations_hex', 'after_relocations_hex'):
        value = document[key]
        if not isinstance(value, str):
            raise ValueError(f'{key}: expected hexadecimal byte text')
        try:
            gaps.append(bytes.fromhex(value))
        except ValueError as error:
            raise ValueError(f'{key}: invalid hexadecimal bytes') from error
    before, after = gaps
    if len(header) + len(before) != fields['e_lfarlc']:
        raise ValueError('MZ bytes before relocation table do not match e_lfarlc')
    result = header + before + table + after
    if len(result) != fields['e_cparhdr'] * 16:
        raise ValueError('MZ header/padding length does not match e_cparhdr')
    MZ.parse_header(result, file_size)
    return result
