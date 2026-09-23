"""Explicit address spaces. Compression mappings are partial, never a delta."""
from dataclasses import dataclass
from common import require

@dataclass(frozen=True)
class Coordinates:
    image_size: int
    unpacked_header: int
    packed_header: int
    ida_base: int = 0x10000

    def load(self, value, space, segment=None):
        if space == 'load_image':
            result = value
        elif space == 'unpacked_mz_file':
            result = value - self.unpacked_header
        elif space == 'restunts_ida':
            result = value - self.ida_base
        elif space == 'segment_offset':
            require(segment is not None and 0 <= segment <= 65535 and 0 <= value <= 65535,
                    'Invalid segment:offset')
            result = segment * 16 + value
        else:
            raise ValueError('No global transform for distributed/packed/driver-integrated coordinates')
        require(0 <= result < self.image_size, 'Coordinate outside pristine initialized image')
        return result

    def from_load(self, value, space, segment=None):
        self.load(value, 'load_image')
        if space == 'load_image':
            return value
        if space == 'unpacked_mz_file':
            return self.unpacked_header + value
        if space == 'restunts_ida':
            return self.ida_base + value
        if space == 'segment_offset':
            require(segment is not None and 0 <= segment <= 65535, 'Segment frame required')
            offset = value - segment * 16
            require(0 <= offset <= 65535, 'Address outside selected segment')
            return offset
        raise ValueError('Unsupported inverse transform')

    def packed_to_load(self, file_offset, commands):
        offset = file_offset - self.packed_header
        matches = []
        for item in commands:
            if item['kind'] in ('copy', 'unchanged_prefix'):
                start = item['packed_load_start']
                if start <= offset < start + item['length']:
                    matches.append(item['load_start'] + offset - start)
        require(len(matches) == 1, 'Packed byte has no unique literal mapping')
        return matches[0]
