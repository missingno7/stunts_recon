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
                                          'seg023', 'seg024', 'seg025', 'seg032'),
                             'ruling': 'reproduction flag set /Oa'},
    'TUFLAG-Ox-seg027': {'flags': ['/AM', '/Ox', '/Gs'],
                         'segments': ('seg027',),
                         'ruling': 'reproduction flag set /Ox'},
    'TUFLAG-Ox-seg030': {'flags': ['/AM', '/Ox', '/Gs'],
                         'segments': ('seg030',),
                         'ruling': 'reproduction set /Ox'},
    'TUFLAG-Ox-seg029': {'flags': ['/AM', '/Ox', '/Gs'],
                         'segments': ('seg029',),
                         'ruling': 'RULING: /Ox'},
    # Member-level entry.  Every seg028 function reloads DS; the source keyword
    # `_loadds` under /AM /Ox /Gs and the flag set /AM /Au /Ox /Gs emit the
    # identical object (NON_DISCRIMINATING, register ruling).  Either spelling
    # is accepted; the recipe's compiler_flags record which one it uses.
    'TUFLAG-Ox-seg028': {'flags': ['/AM', '/Ox', '/Gs'],
                         'alternatives': (['/AM', '/Au', '/Ox', '/Gs'],),
                         'segments': ('seg028',),
                         'ruling': 'RULING: /Ox; DS reload spelling open'},
}


# Register-gated compiler profiles (USER DECISION 2026-09-27): a recipe may use
# the profile only when every function it contributes belongs to the object
# named by a SUPPORTED CC-* register entry, with exactly the ruled flag set.
# The profile's reviewed source extensions (readable `_asm`, optimize pragma
# regions) and its CodeView debug-segment reader policy come with it.
REVIEWED_PROFILES = {
    'CC-MSC600A-seg007': {'profile': 'msc600a-medium-zi',
                          'flags': ['/AM', '/Os', '/Oe', '/Og', '/Gs', '/Zi'],
                          'segments': ('seg007',),
                          'ruling': 'USER DECISION 2026-09-27'},
}


def gated_profile_names():
    return {row['profile'] for row in REVIEWED_PROFILES.values()}


def registered_profile_objects():
    """segment -> (register id, profile, flags) for SUPPORTED profile entries."""
    register = _register()
    result = {}
    for key, reviewed in REVIEWED_PROFILES.items():
        row = register.get(key)
        if row is None or row.get('status') != 'SUPPORTED':
            continue
        text = ' '.join(str(row.get(k, '')) for k in ('hypothesis', 'fails_or_breaks', 'reopen_if'))
        if not (reviewed['ruling'] in text and 'TU-specific' in row.get('scope', '') and
                all(segment in row.get('scope', '') for segment in reviewed['segments'])):
            continue
        for segment in reviewed['segments']:
            require(segment not in result, 'Object has two registered compiler profiles')
            result[segment] = (key, reviewed['profile'], list(reviewed['flags']))
    return result


def profile_gate(recipe):
    """Flags of a recipe using a register-gated profile; refuses everything else."""
    require(recipe.get('kind', 'c') == 'c' and not recipe.get('data_only'),
            'Register-gated compiler profile applies only to C code objects')
    segments = recipe_segments(recipe)
    objects = registered_profile_objects()
    require(len(segments) == 1 and next(iter(segments)) in objects,
            'Compiler profile requires one object with a SUPPORTED register entry (CC-*)')
    key, profile, flags = objects[next(iter(segments))]
    require(recipe.get('profile') == profile and recipe.get('compiler_flags') == flags and
            recipe.get('compiler_flags_register') == key,
            'Register-gated profile recipe differs from the ruled profile/flag set')
    return list(flags)


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


def accepted_flag_sets(key):
    """The ruled set of a register entry plus its reviewed NON_DISCRIMINATING spellings."""
    reviewed = REVIEWED[key]
    return [list(reviewed['flags'])] + [list(f) for f in reviewed.get('alternatives', ())]


def recipe_segments(recipe, image=None):
    """Inventory code segments of every function a C recipe contributes."""
    functions = read_json(ROOT/'evidence/functions.json')['functions']
    # A record-closed prefix lists the full inventory rows of its members.
    listed = recipe.get('members', recipe.get('prefix_members'))
    names = ([m['name'] for m in listed] if listed is not None else [recipe['id']])
    segments = set()
    for name in names:
        rows = [f for f in functions if f.get('name') == name]
        if not rows:
            # A reviewed new inventory entry (e.g. a verified-neighbours gap
            # entry) exists only in the overlay, which carries its segment;
            # its proof is rechecked by function_evidence on every use.
            rows = [f for f in read_json(ROOT/'layout/function-evidence.json')['functions']
                    if f.get('name') == name and f.get('gap_entry_proof')]
        members = ([m for m in listed if m['name'] == name]
                   if listed is not None else [recipe])
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
    if recipe.get('profile') in gated_profile_names():
        return profile_gate(recipe)
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
    require(flags in accepted_flag_sets(key) and recipe.get('compiler_flags_register') == key,
            'compiler_flags differ from the registered object flag set')
    return list(flags)


def object_control_flags(recipe):
    """Registered object flags a canonical-flag C recipe must also reproduce under."""
    if recipe.get('kind', 'c') != 'c' or recipe.get('data_only') or 'compiler_flags' in recipe:
        return None
    if recipe.get('profile') in gated_profile_names():
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
