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


DEBUG_SEGMENT_POLICY = 'msc6-codeview-debsym-debtyp-v1'
_DEBUG_SEGMENTS = {'$$SYMBOLS': 'DEBSYM', '$$TYPES': 'DEBTYP'}


def _debug_segment_indexes(data, policy):
    """SEGDEF indexes of MSC 6 /Zi CodeView segments under the reviewed policy.

    Only `$$SYMBOLS` (class DEBSYM) and `$$TYPES` (class DEBTYP), private,
    byte-aligned and outside every group.  LINK without /CO discards them, so
    they never reach the load image; their records stay in the candidate's
    OMF but are excluded from contribution bytes, fixups and relocations."""
    if policy is None:
        return set()
    require(policy == DEBUG_SEGMENT_POLICY, 'Unknown debug segment policy')
    raw = OmfReader().read(data)
    grouped = {i for g in raw.groups for i in g['segment_indices']}
    indexes = set()
    for seg in raw.segment_defs:
        if seg['name'] in _DEBUG_SEGMENTS or seg['class'] in _DEBUG_SEGMENTS.values():
            require(_DEBUG_SEGMENTS.get(seg['name']) == seg['class'] and seg['combine'] == 'private'
                    and seg['alignment'] == 'byte' and seg['index'] not in grouped,
                    'Unexpected debug segment declaration')
            indexes.add(seg['index'])
    return indexes


def read_object(data, *, ledata_policy=None, record_policy=None,
                sparse_zero=None, research_local_symbols=False, debug_segments=None,
                iterated_fixups=False, communals=None):
    """integ37: `iterated_fixups` admits FIXUPPs over iterated LIDATA content
    (expanded to every repetition, as LINK applies them); `communals` admits
    COMDEF records only when they declare exactly these names (integ39: True
    admits any, for pinned-tool fixtures and diagnostics only)."""
    debug_indexes = _debug_segment_indexes(data, debug_segments)
    if ledata_policy is not None:
        require(set(ledata_policy) == {'mode', 'module_sha256', 'records'}
                and ledata_policy['mode'] == 'pinned-ordered-ledata-v1'
                and ledata_policy['module_sha256'] == sha(data), 'Invalid pinned LEDATA policy or module identity')
    at, ended, first = 0, False, True
    exceptional_checksum_seen = False
    initialized = {}
    writes, had_overlap = [], False
    write_payloads = []
    debug_writes = []
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
        require(kind != 0xB0 or communals is not None,
                'COMDEF communal allocation is deferred: no reviewed linker allocation/ownership rule')
        if kind == 0xB0:
            allowed.add(0xB0)
        require(not ended and length >= 1 and end <= len(data) and kind in allowed,
                f'Invalid/unsupported OMF record {kind:02x}')
        require(not first or kind == 0x80, 'Object must start with THEADR')
        checksum_ok = data[end - 1] == 0 or sum(data[at:end]) & 255 == 0
        if not checksum_ok:
            require(record_policy == {
                'mode':'crt0-comment-checksum-v1',
                'module_sha256':'d5b8b4a264adea82a75056189745d9d786e81192af65e9d4713e4ab0a687a973',
                'record_offset':466, 'kind':0x88, 'size':4,
                'body_sha256':'c6ef173229e1869cd33073657375349eaf71a2329203c009ecd0c9c7c8410aad',
                'checksum':0xd1,
            } and sha(data) == record_policy['module_sha256'] and
                    at == record_policy['record_offset'] and kind == record_policy['kind'] and
                    length == record_policy['size'] and sha(data[at+3:end-1]) == record_policy['body_sha256'] and
                    data[end-1] == record_policy['checksum'], 'OMF checksum mismatch')
            exceptional_checksum_seen = True
        require(kind != 0x9C or last_data_kind != 0xA2 or iterated_fixups,
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
            if segment in debug_indexes:
                # CodeView symbol records are rewritten in place; never image bytes.
                debug_writes.append({'segment_index':segment,'offset':offset,'size':length_data,
                                     'sha256':sha(body[pos+2:])})
                first, ended, at = False, kind == 0x8A, end
                continue
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
            require(segment not in debug_indexes, 'LIDATA in a debug segment unsupported')
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
    require(record_policy is None or exceptional_checksum_seen,
            'Unused exceptional OMF checksum policy')
    if ledata_policy is not None:
        require(had_overlap and writes == ledata_policy['records'], 'Ordered LEDATA trace differs from reviewed policy')
    obj = OmfReader(communals=communals is not None).read(data)
    require(communals is None or communals is True or [c['name'] for c in obj.communals] == list(communals),
            'COMDEF names differ from the reviewed communal declarations')
    obj.unreferenced_communals = unreferenced_communals(obj)
    obj.debug_segments = []
    if debug_indexes:
        debug_names = {s['name'] for s in obj.segment_defs if s['index'] in debug_indexes}
        require(not any(p['segment'] in debug_names for p in obj.publics + obj.local_publics),
                'Public inside a debug segment')
        require(not any(f.get('target') in debug_names or f.get('frame') in debug_names
                        for f in obj.linker_fixups if f['segment'] not in debug_names),
                'Image fixup refers to a debug segment')
        for seg in obj.segment_defs:
            if seg['index'] in debug_indexes:
                obj.debug_segments.append({
                    'index': seg['index'], 'name': seg['name'], 'class': seg['class'], 'length': seg['length'],
                    'records': [w for w in debug_writes if w['segment_index'] == seg['index']],
                    'fixups': sum(1 for f in obj.linker_fixups if f['segment'] == seg['name'])})
        obj.segment_defs = [s for s in obj.segment_defs if s['index'] not in debug_indexes]
        obj.linker_fixups = [f for f in obj.linker_fixups if f['segment'] not in debug_names]
        obj.fixups = [f for f in obj.fixups if f.get('segment') not in debug_names]
        for name in debug_names:
            obj.segments.pop(name, None)
            obj.segment_lengths.pop(name, None)
    for index,offset,expanded in iterated_payloads:
        segments = [s for s in obj.segment_defs if s['index'] == index]
        require(len(segments) == 1 and
                obj.segment_bytes(segments[0]['name'])[offset:offset+len(expanded)] == expanded,
                'Complete LIDATA expansion differs')
    obj.local_symbol_records = local_symbol_records
    # The exact emitted OMF stays attached: record-level proofs traverse the
    # candidate's own LEDATA/FIXUPP structure, never a reconstruction of it.
    obj.omf_bytes = bytes(data)
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
            occupied = initialized[seg['index']]
            policy=(sparse_zero or {}).get(seg['name'])
            exact_ranges=(policy is not None and 'initialized_ranges' in policy and
                          set(policy)=={'initialized_ranges','declared_length'} and
                          policy['declared_length']==seg['length'] and
                          all(type(row) is list and len(row)==2 and
                              0<=row[0]<row[1]<=seg['length']
                              for row in policy['initialized_ranges']) and
                          occupied==set().union(*(set(range(*row))
                                                    for row in policy['initialized_ranges'])))
            require(occupied == set(range(seg['length'])) or
                    (sparse_zero is not None and seg['name'] in sparse_zero and
                     sparse_zero[seg['name']] == {'initialized_prefix':len(occupied),
                                                 'declared_length':seg['length']} and
                     occupied == set(range(len(occupied))) and
                     len(occupied) < seg['length']) or exact_ranges,
                    'Holes or overflow in initialized segment')
    require(set(sparse_zero or {}) <= {s['name'] for s in obj.segment_defs},
            'Unknown sparse zero segment')
    for name, policy in (sparse_zero or {}).items():
        require(name in obj.segments and obj.segment_length(name)==policy['declared_length'],
                'Sparse zero policy differs')
        if 'initialized_prefix' in policy:
            require(len(obj.segment_bytes(name))==policy['initialized_prefix'],
                    'Sparse zero prefix differs')
            obj.segments[name] += bytes(policy['declared_length']-policy['initialized_prefix'])
        else:
            require('initialized_ranges' in policy and len(obj.segment_bytes(name))==policy['declared_length']
                    and all(obj.segment_bytes(name)[at]==0 for at in range(policy['declared_length'])
                            if not any(lo<=at<hi for lo,hi in policy['initialized_ranges'])),
                    'Sparse zero interior differs')
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

def unreferenced_communals(obj):
    """integ39: COMDEF names no FIXUPP of the object targets (a tentative
    definition the object itself never uses)."""
    used = {f['target'] for f in obj.linker_fixups if f['target_kind'] == 'external'}
    used |= {f['frame'] for f in obj.linker_fixups if f.get('frame_kind') == 'external'}
    return [c['name'] for c in getattr(obj, 'communals', []) if c['name'] not in used]


def declared_externals(obj):
    """External-index names subject to the binders' use checks.  integ39: an
    unreferenced COMDEF is storage the object declares for the accepted
    communal unit, not a binding; it is checked against the recipe's
    `communal_declarations` and the unit (tools/communal_unit.py) instead."""
    skip = set(getattr(obj, 'unreferenced_communals', None) or unreferenced_communals(obj))
    return {n for n in obj.externals if n not in skip}


def _data_record_spans(data):
    """(segment index, offset, length) of every LEDATA and bounded-expanded LIDATA record."""
    spans, at = [], 0
    while at + 3 <= len(data):
        kind, length = data[at], struct.unpack_from('<H', data, at + 1)[0]
        body = data[at+3:at+3+length-1]
        if kind in (0xA0, 0xA2):
            segment, pos = OmfReader._index(body, 0)
            require(pos + 2 <= len(body), 'Truncated data record offset')
            offset = struct.unpack_from('<H', body, pos)[0]; pos += 2
            if kind == 0xA0:
                size = len(body) - pos
            else:
                size = 0
                while pos < len(body):
                    part, pos = _iterated_data(body, pos)
                    size += len(part)
            spans.append((segment, offset, size))
        at += 3 + length
    return spans


def msc_alignment_sparse_zero(data):
    """Research helper: the `initialized_ranges` policy of an MSC object whose
    `_DATA`/`CONST` segments have only word-alignment holes, or None.

    Used by diagnostic compiles (prefix_proof research, tubench) to read such
    objects consistently with the reviewed recipe policy; production reads the
    policy from the recipe (recipe_sparse_zero) and never derives it.

    MSC 5.10 emits the zero tail of a partially initialized aggregate
    (`char a[82] = {0};`) as a LIDATA record; its bounded expansion counts as
    initialized coverage (integ26). A FIXUPP over LIDATA stays refused by
    read_object."""
    raw = OmfReader().read(data)
    names = {d['index']: d['name'] for d in raw.segment_defs}
    lengths = {d['name']: d['length'] for d in raw.segment_defs}
    covered = {}
    for index, offset, size in _data_record_spans(data):
        covered.setdefault(names.get(index), set()).update(range(offset, offset + size))
    policy = {}
    for name, occupied in covered.items():
        if occupied == set(range(lengths[name])):
            continue
        if name not in ('_DATA', 'CONST'):
            return None
        ranges, at = [], 0
        for offset in sorted(occupied):
            if ranges and offset == ranges[-1][1]:
                ranges[-1][1] += 1
            else:
                ranges.append([offset, offset + 1])
        if not (ranges and ranges[0][0] == 0 and ranges[-1][1] == lengths[name] and
                all(b[0] == a[1] + 1 and b[0] % 2 == 0 for a, b in zip(ranges, ranges[1:]))):
            return None
        policy[name] = {'initialized_ranges': ranges, 'declared_length': lengths[name]}
    return policy or None


def recipe_sparse_zero(recipe):
    """Reviewed declared-but-uninitialised bytes of a C object (integ25).

    Two MSC 5.10 forms only, both zero in the linked image and compared with
    the oracle like every other byte:
    * the CODE object tail: `initialized_prefix` of the complete CODE SEGDEF,
      tied to a reviewed `object_tail` (e.g. the /Ol `90 90` tail plus one
      declared uninitialised byte);
    * secondary `_DATA`/`CONST` word-alignment holes: `initialized_ranges`
      whose gaps are single bytes before an even (word-aligned) offset.
    Anything else is refused; the returned policy is passed to read_object."""
    policy = recipe.get('sparse_zero')
    if policy is None:
        return None
    require(recipe.get('kind', 'c') == 'c' and type(policy) is dict and policy,
            'Sparse zero policy applies only to reviewed C objects')
    secondary = recipe.get('secondary_dgroup_segments', {})
    for name, row in policy.items():
        if name == recipe['object_segment']:
            tail = recipe.get('object_tail')
            require(tail is not None and set(row) == {'initialized_prefix', 'declared_length'} and
                    row['declared_length'] == recipe['end'] - recipe['start'] and
                    row['declared_length'] - row['initialized_prefix'] ==
                    tail.get('declared_uninitialized'),
                    'CODE sparse zero must be the reviewed object tail')
        else:
            spec = secondary.get(name)
            ranges = row.get('initialized_ranges')
            # A record-closed prefix need not own its TU data; the policy then
            # only lets the whole candidate object be read (length: SEGDEF).
            unowned_prefix = spec is None and 'prefix_of_object' in recipe
            require(name in ('_DATA', 'CONST') and (spec is not None or unowned_prefix) and
                    set(row) == {'initialized_ranges', 'declared_length'} and
                    type(row.get('declared_length')) is int and
                    (unowned_prefix or row['declared_length'] == spec['end'] - spec['start']) and
                    type(ranges) is list and ranges and
                    all(type(r) is list and len(r) == 2 and all(type(x) is int for x in r) and
                        r[0] < r[1] for r in ranges) and
                    ranges[0][0] == 0 and ranges[-1][1] == row['declared_length'] and
                    all(b[0] == a[1] + 1 and b[0] % 2 == 0 for a, b in zip(ranges, ranges[1:])),
                    'Secondary sparse zero allows only MSC word-alignment holes')
    return policy


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
