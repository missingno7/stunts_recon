"""Fail-closed OMF front end adapted from Empires reconstruct.read_object.

The generic reader is retained separately, with its upstream provenance.
Production uses this stricter surface and explicitly bounded binding modes.
"""
import struct
from common import require, sha
from omf import OmfReader

def read_object(data, *, ledata_policy=None):
    if ledata_policy is not None:
        require(set(ledata_policy) == {'mode', 'module_sha256', 'records'}
                and ledata_policy['mode'] == 'pinned-ordered-ledata-v1'
                and ledata_policy['module_sha256'] == sha(data), 'Invalid pinned LEDATA policy or module identity')
    at, ended, first = 0, False, True
    initialized = {}
    writes, had_overlap = [], False
    allowed = {0x80, 0x88, 0x8A, 0x8C, 0x90, 0x94, 0x96, 0x98, 0x9A, 0x9C, 0xA0}
    while at < len(data):
        require(at + 3 <= len(data), 'Truncated OMF record header')
        kind, length = data[at], struct.unpack_from('<H', data, at + 1)[0]
        end = at + 3 + length
        require(not ended and length >= 1 and end <= len(data) and kind in allowed,
                f'Invalid/unsupported OMF record {kind:02x}')
        require(not first or kind == 0x80, 'Object must start with THEADR')
        require(data[end - 1] == 0 or sum(data[at:end]) & 255 == 0, 'OMF checksum mismatch')
        if kind==0xA0:
            body=data[at+3:end-1]
            require(len(body)>=3,'Truncated LEDATA')
            segment,pos=OmfReader._index(body,0)
            require(pos+2<=len(body),'Truncated LEDATA offset')
            offset=struct.unpack_from('<H',body,pos)[0]
            length_data=len(body)-pos-2
            span=set(range(offset,offset+length_data))
            previous=initialized.setdefault(segment,set())
            overlap=sorted(span & previous)
            writes.append({'record_offset':at, 'segment_index':segment, 'offset':offset,
                           'size':length_data, 'sha256':sha(body[pos+2:]), 'overlap_offsets':overlap})
            require(not overlap or ledata_policy is not None,'Overlapping LEDATA')
            had_overlap = had_overlap or bool(overlap)
            previous.update(span)
        first, ended, at = False, kind == 0x8A, end
    require(ended, 'Missing OMF MODEND')
    if ledata_policy is not None:
        require(had_overlap and writes == ledata_policy['records'], 'Ordered LEDATA trace differs from reviewed policy')
    obj = OmfReader().read(data)
    require(not had_overlap or not obj.linker_fixups, 'Overlapping LEDATA with fixups is unsupported')
    names = [s['name'] for s in obj.segment_defs]
    require(len(names) == len(set(names)), 'Duplicate SEGDEF names unsupported')
    for seg in obj.segment_defs:
        require(not seg['use_32bit_offset'],'32-bit SEGDEF unsupported')
        require(not seg['big'],'64KiB BIG SEGDEF unsupported; zero length is not zero storage')
        if seg['index'] in initialized:
            require(initialized[seg['index']]==set(range(seg['length'])),'Holes or overflow in initialized segment')
    for public in obj.publics:
        require(public['segment'] in obj.segment_lengths and 0<=public['offset']<=obj.segment_lengths[public['segment']],'Public outside segment')
    used_fixups={}
    for fix in obj.linker_fixups:
        require(fix['segment'] in obj.segments and 0<=fix['offset'] and fix['offset']+fix['width']<=len(obj.segments[fix['segment']]),'Fixup outside emitted segment')
        span=set(range(fix['offset'],fix['offset']+fix['width']))
        previous=used_fixups.setdefault(fix['segment'],set())
        require(not span & previous,'Overlapping fixups')
        previous.update(span)
    for name, payload in obj.segments.items():
        require(len(payload) <= obj.segment_lengths[name], 'OMF data exceeds SEGDEF')
    for fix in obj.linker_fixups:
        require(fix['target_kind'] != 'absolute', 'Absolute/undefined target thread unsupported')
        require(not str(fix['target']).startswith('?'), 'Undefined OMF target')
    return obj

def extract_no_fixups(obj, segment, public, length):
    """First production subset: entire emitted text contribution, no fixups.

    A fixup-bearing candidate is blocked, never masked or accepted as unchecked.
    The full object reader still exposes fixups for supervisor research.
    """
    require(not obj.linker_fixups, 'Binding blocked: fixup-bearing production objects not yet supported')
    require(obj.externals in ([], ['__acrtused', public]), 'Unexpected external declarations; only unused MSC CRT/self marker allowed')
    publics = obj.publics_in(segment)
    require(publics == [{'name': public, 'segment': segment, 'offset': 0}], 'Public extent mismatch')
    require(obj.segment_length(segment) == length, 'SEGDEF length differs from complete candidate extent')
    payload = obj.segment_bytes(segment)
    require(len(payload) == length, 'Incomplete candidate LEDATA')
    for name, size in obj.segment_lengths.items():
        require(name == segment or size == 0, 'Unexpected additional initialized/BSS contribution')
    return payload
