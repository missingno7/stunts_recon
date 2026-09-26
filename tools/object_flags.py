"""Reviewed per-object compiler flag sets (reproduction profiles, not provenance).

The canonical C profile stays `/AM /O /Gs`.  A recipe may carry
`compiler_flags` only when every function it contributes belongs to one object
covered by a SUPPORTED `TUFLAG-*` register entry, and the flags equal that
entry's ruled set.  The set applies to the whole object: an accepted C
contribution inside a flagged object that keeps the canonical flags must also
reproduce exactly under the object's registered set (checked on every build).
"""
from common import ROOT, read_json, require

# Reviewed mapping from register entry to its ruled flag set and objects.  The
# register itself is re-read at every use: a status change away from SUPPORTED,
# a changed scope, or a changed ruling text refuses the override.
REVIEWED = {
    'TUFLAG-Oa-seg023-025': {'flags': ['/AM', '/Oa', '/Gs'],
                             'segments': ('seg014', 'seg015', 'seg019', 'seg021', 'seg022',
                                          'seg023', 'seg024', 'seg025'),
                             'ruling': 'reproduction flag set /Oa'},
    'TUFLAG-Ox-seg027': {'flags': ['/AM', '/Ox', '/Gs'],
                         'segments': ('seg027',),
                         'ruling': 'reproduction flag set /Ox'},
    'TUFLAG-Ox-seg030': {'flags': ['/AM', '/Ox', '/Gs'],
                         'segments': ('seg030',),
                         'ruling': 'reproduction set /Ox'},
}


def _register():
    document = read_json(ROOT/'evidence/toolchain-hypotheses.json')
    rows = document['hypotheses']
    return {row['id']: row for row in (rows if isinstance(rows, list) else rows.values())}


def registered_objects():
    """segment name -> (register id, flags) for currently SUPPORTED entries."""
    register = _register()
    result = {}
    for key, reviewed in REVIEWED.items():
        row = register.get(key)
        if row is None or row.get('status') != 'SUPPORTED':
            continue
        if not (reviewed['ruling'] in row.get('reopen_if', '') + ' ' + row.get('hypothesis', '')
                and all(segment in row.get('scope', '') for segment in reviewed['segments'])
                and 'TU-specific' in row.get('scope', '')):
            continue
        for segment in reviewed['segments']:
            require(segment not in result, 'Object has two registered flag sets')
            result[segment] = (key, list(reviewed['flags']))
    return result


def recipe_segments(recipe, image=None):
    """Inventory code segments of every function a C recipe contributes."""
    functions = read_json(ROOT/'evidence/functions.json')['functions']
    names = ([m['name'] for m in recipe['members']] if 'members' in recipe else [recipe['id']])
    segments = set()
    for name in names:
        rows = [f for f in functions if f.get('name') == name]
        members = ([m for m in recipe['members'] if m['name'] == name]
                   if 'members' in recipe else [recipe])
        if not rows and members and all(m.get('local_symbol') for m in members):
            continue  # reviewed local helper; its group's other members place it
        require(len(rows) == 1 and rows[0].get('segment'),
                'Per-object flags need an inventory segment for every member')
        row = rows[0]
        require(all((m['start'], m['end']) == (row['start'], row['end']) for m in members),
                'Per-object flag member extent differs from inventory')
        segments.add(row['segment'])
    return segments


def recipe_flags(recipe):
    """Compiler flags for a C recipe; None selects the pinned profile flags."""
    if 'compiler_flags' not in recipe:
        return None
    require(recipe.get('kind', 'c') == 'c' and recipe.get('profile') == 'msc510-medium' and
            not recipe.get('data_only'),
            'Per-object compiler flags apply only to MSC 5.10 C code objects')
    flags = recipe['compiler_flags']
    require(type(flags) is list and all(type(f) is str for f in flags),
            'Invalid compiler_flags')
    segments = recipe_segments(recipe)
    objects = registered_objects()
    require(len(segments) == 1 and next(iter(segments)) in objects,
            'compiler_flags require one object with a SUPPORTED TUFLAG register entry')
    key, registered = objects[next(iter(segments))]
    require(flags == registered and recipe.get('compiler_flags_register') == key,
            'compiler_flags differ from the registered object flag set')
    return list(flags)


def object_control_flags(recipe):
    """Registered object flags a canonical-flag C recipe must also reproduce under."""
    if recipe.get('kind', 'c') != 'c' or recipe.get('data_only') or 'compiler_flags' in recipe:
        return None
    # Object membership by extent: every inventory function the contribution
    # overlaps, independent of recipe spellings.
    start, end = recipe.get('start'), recipe.get('end')
    if 'members' in recipe:
        start, end = recipe['members'][0]['start'], recipe['members'][-1]['end']
    if type(start) is not int or type(end) is not int:
        return None
    functions = read_json(ROOT/'evidence/functions.json')['functions']
    segments = {f['segment'] for f in functions if type(f.get('start')) is int and
                type(f.get('end')) is int and f['start'] < end and start < f['end'] and
                f.get('segment')}
    objects = registered_objects()
    flagged = {objects[s][0]: objects[s][1] for s in segments if s in objects}
    if not flagged:
        return None
    require(len(flagged) == 1 and len(segments) == 1,
            'C contribution spans a flagged object boundary')
    return next(iter(flagged.values()))


def same_object(left, right):
    """Complete emitted object identity used by the per-object flag control."""
    return ({n: bytes(b) for n, b in left.segments.items()} ==
            {n: bytes(b) for n, b in right.segments.items()} and
            left.segment_defs == right.segment_defs and left.groups == right.groups and
            left.publics == right.publics and left.externals == right.externals and
            left.linker_fixups == right.linker_fixups and
            left.local_symbol_records == right.local_symbol_records)
