"""Fail-closed OMF front end adapted from Empires reconstruct.read_object.

The generic reader is retained separately, with its upstream provenance.
Production uses this stricter surface and explicitly bounded binding modes.
"""
import struct
from common import require, sha
from omf import OmfReader


def _iterated_data(body, at, depth=0):
    """Bounded LIDATA16 expansion; relocation of iterated fields is unsupported."""
    require(depth <= 16 and at+4 <= len(body), 'Malformed/deep LIDATA block')
    repeat,count = struct.unpack_from('<HH',body,at); at += 4
    if count:
        parts = []
        size = 0
        for _ in range(count):
            part,at = _iterated_data(body,at,depth+1)
            size += len(part)
            require(size <= 65535, 'LIDATA expansion exceeds 16-bit extent')
            parts.append(part)
        data = b''.join(parts)
    else:
        require(at < len(body), 'Truncated LIDATA leaf')
        size = body[at]; at += 1
        require(at+size <= len(body), 'Truncated LIDATA literal')
        data = body[at:at+size]; at += size
    require(len(data)*repeat <= 65535, 'LIDATA expansion exceeds 16-bit extent')
    return data*repeat,at


def read_object(data, *, ledata_policy=None, research_local_symbols=False):
    if ledata_policy is not None:
        require(set(ledata_policy) == {'mode', 'module_sha256', 'records'}
                and ledata_policy['mode'] == 'pinned-ordered-ledata-v1'
                and ledata_policy['module_sha256'] == sha(data), 'Invalid pinned LEDATA policy or module identity')
    at, ended, first = 0, False, True
    initialized = {}
    writes, had_overlap = [], False
    write_payloads = []
    last_data_kind = None
    iterated_payloads = []
    allowed = {0x80, 0x88, 0x8A, 0x8C, 0x90, 0x94, 0x96, 0x98, 0x9A, 0x9C, 0xA0, 0xA2}
    # MSC emits local externals/publics for static same-TU helpers. Their
    # names remain object-private and are checked after full OMF decoding.
    allowed.update({0xB4, 0xB6})
    local_symbol_records = []
    while at < len(data):
        require(at + 3 <= len(data), 'Truncated OMF record header')
        kind, length = data[at], struct.unpack_from('<H', data, at + 1)[0]
        end = at + 3 + length
        require(kind != 0xB0,
                'COMDEF communal allocation is deferred: no reviewed linker allocation/ownership rule')
        require(not ended and length >= 1 and end <= len(data) and kind in allowed,
                f'Invalid/unsupported OMF record {kind:02x}')
        require(not first or kind == 0x80, 'Object must start with THEADR')
        require(data[end - 1] == 0 or sum(data[at:end]) & 255 == 0, 'OMF checksum mismatch')
        require(kind != 0x9C or last_data_kind != 0xA2,
                'FIXUPP over iterated LIDATA requires expanded relocation proof')
        if kind in (0xB4, 0xB6):
            local_symbol_records.append({'kind': 'LEXTDEF' if kind == 0xB4 else 'LPUBDEF',
                                         'record_offset': at, 'body_sha256': sha(data[at+3:end-1])})
        if kind==0xA0:
            last_data_kind = kind
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
            write_payloads.append({'segment_index':segment, 'offset':offset,
                                   'data':bytes(body[pos+2:])})
            had_overlap = had_overlap or bool(overlap)
            previous.update(span)
        if kind == 0xA2:
            last_data_kind = kind
            body = data[at+3:end-1]
            segment,pos = OmfReader._index(body,0)
            require(pos+2 <= len(body), 'Truncated LIDATA offset')
            offset = struct.unpack_from('<H',body,pos)[0]; pos += 2
            expanded = bytearray()
            while pos < len(body):
                part,pos = _iterated_data(body,pos)
                expanded.extend(part)
                require(offset+len(expanded) <= 65535, 'LIDATA expansion exceeds 16-bit extent')
            require(expanded, 'Empty LIDATA record')
            span = set(range(offset,offset+len(expanded)))
            previous = initialized.setdefault(segment,set())
            require(not span.intersection(previous), 'Overlapping LIDATA unsupported')
            previous.update(span)
            iterated_payloads.append((segment,offset,bytes(expanded)))
        first, ended, at = False, kind == 0x8A, end
    require(ended, 'Missing OMF MODEND')
    if ledata_policy is not None:
        require(had_overlap and writes == ledata_policy['records'], 'Ordered LEDATA trace differs from reviewed policy')
    obj = OmfReader().read(data)
    for index,offset,expanded in iterated_payloads:
        segments = [s for s in obj.segment_defs if s['index'] == index]
        require(len(segments) == 1 and
                obj.segment_bytes(segments[0]['name'])[offset:offset+len(expanded)] == expanded,
                'Complete LIDATA expansion differs')
    obj.local_symbol_records = local_symbol_records
    locals_public = {p['name'] for p in obj.local_publics}
    global_public = {p['name'] for p in obj.publics if p not in obj.local_publics}
    global_external = {name for name, scope in zip(obj.externals, obj.external_scopes)
                       if scope == 'external'}
    require(len(locals_public) == len(obj.local_publics) and
            len(obj.local_externals) == len(set(obj.local_externals)),
            'Duplicate local OMF symbol')
    require(not locals_public.intersection(global_public | global_external) and
            not set(obj.local_externals).intersection(global_public | global_external),
            'Local OMF symbol shadows an external binding')
    require(set(obj.local_externals) <= locals_public,
            'Unresolved local OMF external')
    for fix in obj.linker_fixups:
        if fix['target_kind'] == 'external' and 1 <= fix['target_index'] <= len(obj.external_scopes):
            if obj.external_scopes[fix['target_index'] - 1] == 'local':
                require(fix['target'] in locals_public,
                        'Local FIXUPP does not resolve to an internal public')
    if had_overlap and obj.linker_fixups:
        indexes = {s['name']:s['index'] for s in obj.segment_defs}
        for fix in obj.linker_fixups:
            lo, hi = fix['offset'], fix['offset'] + fix['width']
            records = [w for w in write_payloads if w['segment_index'] == indexes[fix['segment']]
                       and w['offset'] < hi and lo < w['offset'] + len(w['data'])]
            require(any(w['offset'] <= lo and hi <= w['offset'] + len(w['data']) for w in records),
                    'Overlapping LEDATA fixup field is not contained in one source record')
            for pos in range(lo, hi):
                values = [w['data'][pos-w['offset']] for w in records
                          if w['offset'] <= pos < w['offset']+len(w['data'])]
                require(values and len(set(values)) == 1,
                        'Overlapping LEDATA writes disagree at fixup field')
                require(obj.segments[fix['segment']][pos] == values[0],
                        'Merged LEDATA fixup addend differs from pinned writes')
    names = [s['name'] for s in obj.segment_defs]
    require(len(names) == len(set(names)), 'Duplicate SEGDEF names unsupported')
    for seg in obj.segment_defs:
        require(not seg['use_32bit_offset'],'32-bit SEGDEF unsupported')
        require(not seg['big'],'64KiB BIG SEGDEF unsupported; zero length is not zero storage')
        if seg['index'] in initialized:
            require(initialized[seg['index']]==set(range(seg['length'])),'Holes or overflow in initialized segment')
    for public in obj.publics:
        if public['segment'] == '?0':
            # MSC CRT linkage marker: an absolute PUBDEF at 9876h has no
            # storage extent, and cannot satisfy an ordinary code/data fixup.
            require(public['name'] in ('__acrtused','__acrtmsg') and
                    public['offset'] == 0x9876, 'Unknown absolute runtime public')
        else:
            require(public['segment'] in obj.segment_lengths and
                    0<=public['offset']<=obj.segment_lengths[public['segment']],
                    'Public outside segment')
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
    The full object reader exposes fixups for scratch diagnostics.
    """
    require(not obj.linker_fixups, 'Binding blocked: fixup-bearing production objects not yet supported')
    require(obj.externals in ([], ['__acrtused', public]), 'Unexpected external declarations; only unused MSC CRT/self marker allowed')
    publics = obj.publics
    require(publics == [{'name': public, 'segment': segment, 'offset': 0}], 'Public extent mismatch')
    require(obj.segment_length(segment) == length, 'SEGDEF length differs from complete candidate extent')
    payload = obj.segment_bytes(segment)
    require(len(payload) == length, 'Incomplete candidate LEDATA')
    for name, size in obj.segment_lengths.items():
        require(name == segment or size == 0, 'Unexpected additional initialized/BSS contribution')
    return payload
