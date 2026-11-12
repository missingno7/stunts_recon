"""Complete object equivalence for declaration and public-spelling changes.

Segment bytes, extents, public/local/communal declarations and ordered named
fixups remain obligations. Declaration mode records unused import changes. Whole-image, communal placement and historical module selection
must additionally be proven by the caller. Cosmetic edits use raw OBJ equality.
"""
import json
from common import require

def renamed(value, mapping):
    """Exact JSON strings/keys only; no substitution inside prose or file paths."""
    if isinstance(value, str):
        return mapping.get(value, value)
    if isinstance(value, list):
        return [renamed(x, mapping) for x in value]
    if isinstance(value, dict):
        keys = [mapping.get(k, k) for k in value]
        require(len(keys) == len(set(keys)), 'JSON key collision')
        return {mapping.get(k, k): renamed(v, mapping) for k, v in value.items()}
    return value


def first_difference(before, after, path=''):
    """Keep list order and full extents; diagnostics never decide acceptance."""
    if type(before) is not type(after):
        return {'record': path, 'before': repr(before)[:300], 'after': repr(after)[:300]}
    if isinstance(before, dict):
        if before.keys() != after.keys():
            return {'record': path + '.keys', 'before': list(before), 'after': list(after)}
        for key in before:
            diff = first_difference(before[key], after[key], path + '.' + str(key))
            if diff:
                return diff
    elif isinstance(before, (list, tuple, bytes, bytearray)):
        for index, (a, b) in enumerate(zip(before, after)):
            diff = first_difference(a, b, f'{path}[{index}]')
            if diff:
                return diff
        if len(before) != len(after):
            return {'record': path + '.length', 'before': len(before), 'after': len(after)}
    elif before != after:
        return {'record': path, 'before': before, 'after': after}
    return None


def object_view(obj, mapping=None):
    """Complete extents and ordered semantic fixups; declaration multisets.

    Only external table indexes are replaceable. Segment/group indexes and
    fixup methods, widths, addends, locations and order remain obligations.
    COMDEF order is deferred to the mandatory whole-image and BSS LINK gates.
    CodeView segments remain byte-exact (including their diagnostic records).
    """
    mapping = mapping or {}
    fields = ('segments', 'segment_lengths', 'segment_defs', 'groups', 'publics',
              'linker_fixups', 'local_publics', 'local_externals', 'communals', 'comments')
    view = renamed({key: getattr(obj, key, None) for key in fields}, mapping)
    sort = lambda values: sorted(values, key=lambda v: json.dumps(v, sort_keys=True))
    for key in ('publics', 'local_publics', 'local_externals', 'communals'):
        view[key] = sort(view[key] or [])
    view['externals'] = sort([(mapping.get(n, n), scope) for n, scope in
                              zip(obj.externals, obj.external_scopes)])
    require(len(obj.externals) == len(obj.external_scopes), 'Incomplete external scope table')
    for f in view['linker_fixups'] or []:
        for prefix in ('target', 'frame'):
            if f.get(prefix + '_kind') == 'external':
                f.pop(prefix + '_index', None)
    return view


def declaration_records(raw, mapping):
    """Retain PUBDEF group/frame/type and EXTDEF type data the reader omits."""
    from omf import OmfReader
    result = []
    for kind, body in OmfReader.records(raw):
        if kind not in (0x90, 0xB6, 0x8C, 0xB4):
            continue
        at, group, segment, frame = 0, None, None, None
        if kind in (0x90, 0xB6):
            group, at = OmfReader._index(body, at)
            segment, at = OmfReader._index(body, at)
            if segment == 0:
                frame = int.from_bytes(body[at:at+2], 'little'); at += 2
        while at < len(body):
            size = body[at]
            name = body[at+1:at+1+size].decode('ascii'); at += size + 1
            offset = None
            if kind in (0x90, 0xB6):
                offset = int.from_bytes(body[at:at+2], 'little'); at += 2
            type_index, at = OmfReader._index(body, at)
            result.append((kind, group, segment, frame, mapping.get(name, name), offset, type_index))
    return sorted(result, key=repr)


def nonsemantic_records(compiled, debug_policy=None):
    """Retain every record not represented by the complete decoded view.

    The explicit MSC6 policy permits LINNUM and the contents/lengths of the
    two reviewed private CodeView segments to differ. Their SEGDEF attributes,
    indexes and names remain exact. The production decoder has already refused
    unknown records, unsupported fixups and image references into debug data.
    """
    from omf import OmfReader
    from object_probe import DEBUG_SEGMENT_POLICY
    require(debug_policy in (None, DEBUG_SEGMENT_POLICY), 'Unknown debug comparison policy')
    represented = {0x8C, 0xB4, 0x90, 0xB6, 0xB0, 0x9C, 0xA0, 0xA2}
    debug = {s['index']: s for s in getattr(compiled[0], 'debug_segments', [])}
    if debug_policy:
        require(debug and {s['name'] for s in debug.values()} == {'$$SYMBOLS', '$$TYPES'},
                'Debug comparison policy requires both reviewed CodeView segments')
    result, segment_index = [], 0
    for kind, body in OmfReader.records(compiled[1]):
        if kind == 0x98:
            segment_index += 1
            if debug_policy and segment_index in debug:
                # Only the two-byte SEGDEF length is debug-dependent.
                result.append((kind, body[:1] + b'\0\0' + body[3:]))
                continue
        if debug_policy and kind == 0x94:
            continue
        if kind not in represented:
            result.append((kind, body))
    return result


def compare_objects(before, after, mapping, *, debug_policy=None, declarations=False):
    # These objects have passed the strict production decoder, with no unknown
    # OMF records or unsupported fixups. Decoded bytes and fixups replace record
    # encoding equality, not proof coverage. Non-semantic records stay exact.
    old_view, new_view = object_view(before[0], mapping), object_view(after[0])
    old_records, new_records = declaration_records(before[1], mapping), declaration_records(after[1], {})
    if declarations:
        # A conventional API header can declare unused services. Their EXTDEFs
        # may be added/removed; every emitted fixup still resolves by name and
        # every shared declaration retains its scope/type. No local/public or
        # communal declaration is exempted. The caller MUST prove the same
        # loaded historical modules, exact executable and BSS placement.
        old_external = {n for n, scope in old_view['externals'] if scope == 'external'}
        new_external = {n for n, scope in new_view['externals'] if scope == 'external'}
        changed = old_external ^ new_external
        old_view['externals'] = [r for r in old_view['externals'] if not (r[0] in changed and r[1] == 'external')]
        new_view['externals'] = [r for r in new_view['externals'] if not (r[0] in changed and r[1] == 'external')]
        old_records = [r for r in old_records if not (r[0] == 0x8C and r[4] in changed)]
        new_records = [r for r in new_records if not (r[0] == 0x8C and r[4] in changed)]
    diff = first_difference(old_view, new_view, 'object')
    declaration_diff = first_difference(old_records, new_records, 'declarations')
    from omf import OmfReader
    debug_names = lambda obj: [d['name'] for d in getattr(obj, 'debug_segments', [])]
    debug = lambda compiled: {n: OmfReader(communals=True).read(compiled[1]).segments.get(n, b'')
                              for n in debug_names(compiled[0])}
    debug_diff = (None if debug_policy else
                  first_difference(debug(before), debug(after), 'object.debug_bytes'))
    record_diff = first_difference(nonsemantic_records(before, debug_policy),
                                   nonsemantic_records(after, debug_policy), 'records')
    if diff or declaration_diff or record_diff or debug_diff:
        return {'semantic': diff or declaration_diff or debug_diff,
                'omf': record_diff or declaration_diff or diff or debug_diff}
    return None


def refresh_expectations(recipe, obj):
    """Record the fresh emitted order/indexes only after semantic equivalence."""
    import copy
    recipe = copy.deepcopy(recipe)
    declarations = {'segments': obj.segment_defs, 'groups': obj.groups,
                    'publics': obj.publics, 'externals': obj.externals}
    if 'object_declarations' in recipe:
        recipe['object_declarations'] = declarations
    if 'declarations' in recipe.get('binding', {}):
        recipe['binding']['declarations'] = declarations
    if 'expected_fixups' in recipe:
        recipe['expected_fixups'] = obj.linker_fixups
    if 'communal_declarations' in recipe:
        recipe['communal_declarations'] = [{'name': c['name'], 'size': c['length']} for c in obj.communals]
    return recipe

