"""Strict COMDEF acceptance: the whole LINK c_common unit at once (integ39).

LINK 3.65 allocates every near communal (OMF COMDEF 62h: an MSC tentative
definition `T x;` or a MASM `COMM NEAR x:type:n`) in its own paragraph-aligned
segment `c_common` (class BSS, DGROUP) after the BSS-class sections, in its
symbol-table walk (tools/communal_order.py, DIAGNOSTIC).  The order depends on
every name of the program, so no communal can be owned alone: the unit
[c_common start, _end) is accepted in one transaction or stays one raw unit.

One `bss_owners` row of kind LINK_COMMUNAL (placement link-communal-v1) owns the
unit.  It lists every communal: OMF name, address, size, declaring owners and a
size basis.  The row is accepted only when all four conditions hold:

(a) declarers: every declaring unit is an accepted owner that is itself exact
    (a C recipe's tentative definitions, an ASM module's COMM NEAR, the pinned
    `_file.c` member); the declarers are re-derived from the objects linked, the
    size is the largest declared size, and no other object declares a COMDEF;
(b) placement: one real LINK run that reproduces the image places every
    communal: its MAP public, the MAP c_common segment, and the linked operand
    of every referencing FIXUPP in every linked accepted object;
(c) names: every name is the reviewed program-wide registry name of its address
    (or the pinned runtime member's own COMDEF name); placeholder or
    address-derived spellings are refused;
(d) sizes grounded: `declared-type` (the declaration text is in a declaring
    source), `pinned-member`, `absorbed-gap` (NOTES ruling: an unreferenced gap
    absorbed only into a runtime-indexed array whose element size divides the
    combined size, with original indexed-access witnesses) or
    `neutral-unreferenced` (no reference anywhere, with a review note).

The c_common paragraph fill before the unit is a LINK_FILL row
(link-communal-paragraph-v1, checked_communal_fill).  Nothing here places
storage: the real link does (tools/reallink.py), and this module decides
whether its result reproduces the claim.
"""
import re
import struct
from pathlib import Path
from common import ROOT, read_json, require

KIND = 'LINK_COMMUNAL'
PLACEMENT = 'link-communal-v1'            # bss_link.COMMUNAL
FILL_BASIS = 'link-communal-paragraph-v1'  # bss_link.COMMUNAL_FILL_BASIS
SCHEMA = 'communal-unit-v1'
SEGMENT = 'c_common'
ROW_KEYS = {'id', 'kind', 'classification', 'placement', 'segment', 'start', 'end', 'communals', 'schema'}
ITEM_KEYS = {'name', 'address', 'size', 'declarers', 'size_basis'}
SIZE_BASES = ('declared-type', 'pinned-member', 'absorbed-gap', 'neutral-unreferenced')
FILL_KEYS = {'id', 'kind', 'start', 'end', 'basis', 'object'}
XOE_ANCHOR = ('library_dos_crt0dat_118056', 'XOE')
# (c) address-derived / placeholder spellings (IDA labels, solver bucket slots).
_IDA = re.compile(r'(?:byte|word|dword|unk|stru|asc|off|seg|flt|dbl|data|loc|sub)_[0-9A-Fa-f]{3,6}', re.I)


def is_placeholder(name):
    """Address-derived or measurement-only spelling (never a reviewed name)."""
    bare = name.lstrip('_')
    if _IDA.fullmatch(bare) or re.search(r'(?:^|_)slot\d|placeholder', bare, re.I):
        return True
    tail = bare.rsplit('_', 1)
    return len(tail) == 2 and bool(re.fullmatch(r'[0-9A-Fa-f]{4,6}', tail[1])) and bool(re.search(r'\d', tail[1]))


def _norm(text):
    return re.sub(r'\s+', ' ', text).strip()


# --------------------------------------------------------------------------- fill
def fill_row(start, end):
    return {'id': f'fill_{start:05x}_{end:05x}', 'kind': 'LINK_FILL', 'start': start, 'end': end,
            'basis': FILL_BASIS, 'object': '%s:%s' % XOE_ANCHOR}


def checked_communal_fill(owner, manifest):
    """Re-derive the c_common paragraph fill (never from a stored claim).

    * it starts where the BSS-class sections end: the CRT0DAT XOE marker's
      linked real-link-v1 storage row (zero-length, after the complete `_BSS`
      class) and the end of the preceding bss_owners row;
    * it ends at the next paragraph, 1..15 bytes later: LINK makes `c_common`
      PARAGRAPH aligned (fixture tests/fixtures/masm510_comm_near_fixture.json);
    * the communal unit (raw or accepted) starts exactly at its end.
    The staged real link proves it (reallink.bss_placement: MAP c_common)."""
    start, end = owner['start'], owner['end']
    require(set(owner) == FILL_KEYS and owner.get('kind') == 'LINK_FILL' and owner.get('basis') == FILL_BASIS and
            owner == fill_row(start, end), 'Unsupported communal paragraph fill form')
    require(type(start) is int and 0 < end - start < 16 and end % 16 == 0 and end == -(-start // 16) * 16,
            'Communal fill must reach exactly the next paragraph')
    rows = manifest.get('bss_owners', [])
    at = next(i for i, o in enumerate(rows) if o is owner or o == owner)
    require(0 < at < len(rows) - 1 and rows[at - 1]['end'] == start and rows[at + 1]['start'] == end and
            (rows[at + 1].get('raw_form') == 'communal-unit' or rows[at + 1].get('kind') == KIND),
            'Communal fill is not between the BSS-class sections and c_common')
    member, segment = XOE_ANCHOR
    parents = [o for o in manifest['owners'] if o['id'] == member]
    xoe = (parents[0].get('binding', {}).get('storage', {}).get(segment) if len(parents) == 1 else None) or {}
    require(xoe.get('ownership') == 'linked' and xoe.get('anchor', {}).get('kind') == 'real-link-v1' and
            xoe.get('start') == xoe.get('end') == start,
            'Communal fill does not start at the linked CRT0DAT XOE marker')
    return {'fill': [start, end], 'after': '%s:%s' % XOE_ANCHOR, 'next_segment': SEGMENT, 'basis': FILL_BASIS}


# --------------------------------------------------------------------------- row form
def _align(at, size):
    return at + (at & 1) if size % 2 == 0 else at


def check_row_form(row, layout=None):
    """Structural form of one LINK_COMMUNAL row (no link run): complete tiling of
    [start, end) by the LINK allocation rule (an even-sized communal starts even),
    unique names, known size bases.  Returns the item list."""
    layout = layout or read_json(ROOT / 'layout/data-symbols.json')
    require(set(row) == ROW_KEYS and row['kind'] == KIND and row['placement'] == PLACEMENT and
            row['segment'] == SEGMENT and row['schema'] == SCHEMA and row['classification'] == KIND and
            row['start'] % 16 == 0 and row['end'] == layout['bss_end'],
            'Communal unit row form differs')
    items = row['communals']
    require(isinstance(items, list) and items, 'Communal unit lists no communals')
    at, names = row['start'], set()
    for item in items:
        require(isinstance(item, dict) and set(item) == ITEM_KEYS and type(item['size']) is int and
                item['size'] > 0 and isinstance(item['name'], str) and
                re.fullmatch(r'[A-Za-z_$?@][A-Za-z0-9_$?@]{0,254}', item['name']) and
                isinstance(item['declarers'], list) and item['declarers'] == sorted(set(item['declarers'])) and
                item['declarers'] and isinstance(item['size_basis'], dict) and
                item['size_basis'].get('kind') in SIZE_BASES, 'Communal item form differs: %s' % item.get('name'))
        require(item['name'] not in names, 'Duplicate communal name %s' % item['name'])
        names.add(item['name'])
        expected = _align(at, item['size'])
        require(item['address'] == expected and expected - at <= 1,
                'Communal %s is not at its allocation position %d' % (item['name'], expected))
        at = expected + item['size']
    require(at == row['end'], 'Communal allocations do not tile the unit (%d != %d)' % (at, row['end']))
    return items


# --------------------------------------------------------------------------- recipes (a, d)
def recipe_declarations(recipe):
    """The reviewed COMDEF records of a C/ASM recipe: [(name, size)] in record order."""
    rows = recipe.get('communal_declarations')
    if rows is None:
        return None
    require(isinstance(rows, list) and rows and
            all(isinstance(r, dict) and set(r) == {'name', 'size'} and isinstance(r['name'], str) and
                type(r['size']) is int and r['size'] > 0 for r in rows) and
            len({r['name'] for r in rows}) == len(rows), 'Invalid recipe communal_declarations')
    return [(r['name'], r['size']) for r in rows]


def check_object_communals(obj, recipe):
    """(a) The compiled object declares exactly the recipe's near COMDEFs."""
    declared = recipe_declarations(recipe)
    got = [(c['name'], c['kind'], c['length']) for c in getattr(obj, 'communals', [])]
    want = [(n, 'near', s) for n, s in (declared or [])]
    require(got == want, 'Emitted COMDEF records differ from the recipe communal_declarations')


def manifest_declarers(manifest, recipe_of):
    """name -> [(owner id, size, source path or None)] from the accepted owners'
    recipes and the pinned runtime members' own COMDEFs (omf_policy.communals)."""
    out = {}
    for owner in manifest['owners']:
        if owner['kind'] not in ('MATCHING_C', 'MATCHING_ASM') or 'recipe' not in owner:
            continue
        recipe = recipe_of(owner)
        for name, size in recipe_declarations(recipe) or []:
            out.setdefault(name, []).append((owner['id'], size, recipe['source']))
    import runtime_binding
    for member in runtime_binding.runtime_owners(manifest):
        names = member.get('omf_policy', {}).get('communals')
        if names:
            sizes = pinned_member_communals(member)
            require([n for n, _ in sizes] == list(names), 'Pinned member COMDEFs differ from its policy')
            for name, size in sizes:
                out.setdefault(name, []).append((member['id'], size, None))
    return out


def pinned_member_communals(member):
    from compiler import toolchain_path
    from common import sha
    from omf import OmfReader
    data = toolchain_path(member['library']).read_bytes()
    require(sha(data) == member['library_sha256'], 'Pinned library identity differs')
    blobs = [b for n, b in OmfReader().split_library(data) if n == member['module'] and sha(b) == member['module_sha256']]
    require(len(blobs) == 1, 'Pinned member missing')
    obj = OmfReader(communals=True).read(blobs[0])
    require(all(c['kind'] == 'near' for c in obj.communals), 'Pinned member declares far communals')
    return [(c['name'], c['length']) for c in obj.communals]


def accepted_row(manifest):
    rows = [o for o in manifest.get('bss_owners', []) if o.get('kind') == KIND]
    require(len(rows) <= 1, 'More than one communal unit row')
    return rows[0] if rows else None


def registry_names():
    reg = read_json(ROOT / 'layout/names-registry.json')['names']
    return {int(a): r['name'] for a, r in reg.items() if r.get('kind') == 'data'}


def check_manifest(manifest, recipe_of, read_source=None):
    """Recipe-level acceptance of the communal unit (hybrid build, no link run):
    (a) declarers and sizes from the recipes, (c) names, (d) size-basis form and
    source text (`read_source(path)`: the staged candidate bytes during a
    promotion, else the tracked file).  Without an accepted row no recipe may
    declare a COMDEF (a communal is accepted only with its whole unit).
    Returns a receipt."""
    declared = manifest_declarers(manifest, recipe_of)
    row = accepted_row(manifest)
    game = {n: d for n, d in declared.items() if any(src is not None for _, _, src in d)}
    if row is None:
        require(not game, 'Communal declarations are accepted only with the whole communal unit: %s'
                % sorted(game)[:5])
        return None
    items = check_row_form(row)
    problems = []
    problems += _check_declarers(items, {n: [(o, s) for o, s, _ in d] for n, d in declared.items()})
    problems += check_names(items, manifest)
    problems += _check_size_basis_form(items, declared, read_source)
    counts = {k: sum(p.startswith('(%s)' % k) for p in problems) for k in 'abcd'}
    require(not problems, 'Communal unit refused (%d problems; a:%d c:%d d:%d): %s'
            % (len(problems), counts['a'], counts['c'], counts['d'], '; '.join(problems[:6])))
    return {'communal_unit': row['id'], 'communals': len(items), 'bytes': row['end'] - row['start']}


def _check_declarers(items, declared):
    """(a) row declarers == declaring owners; size == largest declared size."""
    problems = []
    names = {i['name'] for i in items}
    for item in items:
        rows = declared.get(item['name'], [])
        if not rows:
            problems.append('(a) %s has no accepted declaring unit' % item['name'])
            continue
        owners = sorted({o for o, _ in rows})
        if owners != item['declarers']:
            problems.append('(a) %s declarers %s differ from %s' % (item['name'], owners, item['declarers']))
        if max(s for _, s in rows) != item['size']:
            problems.append('(a) %s declared size %d differs from %d' % (item['name'], max(s for _, s in rows),
                                                                        item['size']))
    for name in sorted(set(declared) - names):
        problems.append('(a) COMDEF %s declared outside the communal unit' % name)
    return problems


def check_names(items, manifest):
    """(c) reviewed registry names; pinned runtime rows keep the member's own name."""
    problems = []
    registry = registry_names()
    import runtime_binding
    pinned = {n for m in runtime_binding.runtime_owners(manifest) for n in m.get('omf_policy', {}).get('communals', [])}
    for item in items:
        name = item['name']
        if is_placeholder(name):
            problems.append('(c) %s is a placeholder/address-derived spelling' % name)
        elif item['size_basis']['kind'] == 'pinned-member':
            if name not in pinned:
                problems.append('(c) %s is not a pinned member COMDEF' % name)
        elif registry.get(item['address']) is None or '_' + registry[item['address']] != name:
            problems.append('(c) %s is not the registry name of %d (%s)' % (name, item['address'],
                                                                           registry.get(item['address'])))
    return problems


def _check_size_basis_form(items, declared, read_source=None):
    """(d) form and source grounding of each size basis (link-free part)."""
    read_source = read_source or (lambda path: (ROOT / path).read_bytes())
    problems = []
    for item in items:
        basis, name = item['size_basis'], item['name']
        kind = basis['kind']
        sources = [src for _, _, src in declared.get(name, []) if src]
        if kind == 'pinned-member':
            if set(basis) != {'kind'} or sources:
                problems.append('(d) %s pinned-member basis with a source declarer' % name)
            continue
        text = basis.get('declaration')
        if not isinstance(text, str) or not text.strip():
            problems.append('(d) %s lacks its declaration text' % name)
            continue
        if not any(_norm(text) in _norm(read_source(src).decode('latin-1')) for src in sources):
            problems.append('(d) %s declaration %r is not in a declaring source' % (name, text))
        if kind == 'declared-type':
            if set(basis) != {'kind', 'declaration'}:
                problems.append('(d) %s declared-type basis form' % name)
        elif kind == 'neutral-unreferenced':
            if set(basis) != {'kind', 'declaration', 'review_note'} or len(str(basis.get('review_note'))) < 20:
                problems.append('(d) %s neutral communal needs a review note' % name)
        elif kind == 'absorbed-gap':
            gap, element = basis.get('gap'), basis.get('element_size')
            if set(basis) != {'kind', 'declaration', 'element_size', 'gap', 'indexed_sites'} or \
                    type(element) is not int or element <= 0 or item['size'] % element or \
                    not (isinstance(gap, list) and len(gap) == 2 and
                         item['address'] < gap[0] < gap[1] == item['address'] + item['size']) or \
                    not basis.get('indexed_sites'):
                problems.append('(d) %s absorbed gap violates the ruling (runtime-indexed array whose element '
                                'size divides the combined size)' % name)
    return problems


# --------------------------------------------------------------------------- link (a, b, d)
def _addend(f):
    enc = int.from_bytes(bytes.fromhex(f['encoded_addend']), 'little') if f.get('encoded_addend') else 0
    return enc + (f.get('displacement') or 0)


def linked_references(objects, names, linked_image, dgroup_base):
    """Every FIXUPP of a linked object that targets a communal name.

    objects: [{'id', 'obj', 'place': {segment: load address}}].  Returns rows
    with the linked address the real link gave that reference."""
    out = []
    for entry in objects:
        obj = entry['obj']
        for f in obj.linker_fixups:
            if f['target_kind'] != 'external' or f['target'] not in names:
                continue
            base = entry['place'].get(f['segment'])
            row = {'object': entry['id'], 'name': f['target'], 'segment': f['segment'], 'offset': f['offset'],
                   'loc': f['loc'], 'addend': _addend(f), 'site': None, 'linked': None}
            if base is not None:
                site = base + f['offset']
                row['site'] = site
                value = struct.unpack_from('<H', linked_image, site)[0] if site + 2 <= len(linked_image) else None
                if f['loc'] in ('offset16', 'loader-offset16') and not f['self_relative'] and value is not None:
                    row['linked'] = dgroup_base + ((value - row['addend']) & 0xFFFF)
                elif f['loc'] == 'base16' and value is not None:
                    row['linked_frame'] = value * 16
            out.append(row)
    return out


def check_link(items, declared, references, map_publics, map_segments, image, dgroup_base,
               declarer_exact=None):
    """(a, b, d) from one real link.

    declared: name -> [(declaring unit id, size, accepted)] read from the linked
    objects' COMDEF records; references: linked_references(); map_publics:
    {name: address}; map_segments: [{'name', 'start', 'stop'|'end', ...}];
    declarer_exact: unit id -> bool (the linked object is the accepted exact one)."""
    problems = []
    names = {i['name'] for i in items}
    # (a) declarers re-derived from the linked objects
    for name, rows in declared.items():
        for unit, size, accepted in rows:
            if not accepted:
                problems.append('(a) %s declared by non-accepted unit %s' % (name, unit))
            elif declarer_exact is not None and not declarer_exact.get(unit, False):
                problems.append('(a) %s declared by %s, which is not its accepted exact object' % (name, unit))
    problems += _check_declarers(items, {n: [(u, s) for u, s, _ in rows] for n, rows in declared.items()})
    # (b) MAP publics and segment
    seg = [s for s in map_segments if s['name'] == SEGMENT]
    start = items[0]['address']
    end = items[-1]['address'] + items[-1]['size']
    if len(seg) != 1 or seg[0]['start'] != start or seg[0].get('stop', seg[0].get('end', 0) - 1) != end - 1:
        problems.append('(b) MAP c_common %s differs from [%d,%d)' % (seg, start, end))
    for item in items:
        got = map_publics.get(item['name'])
        if got != item['address']:
            problems.append('(b) %s linked at %s, claimed %d' % (item['name'], got, item['address']))
    by_name = {}
    for ref in references:
        by_name.setdefault(ref['name'], []).append(ref)
        if ref['site'] is None:
            problems.append('(b) %s reference in %s:%s not placed' % (ref['name'], ref['object'], ref['segment']))
        elif ref['loc'] == 'base16':
            if ref.get('linked_frame') != dgroup_base:
                problems.append('(b) %s segment reference at %d is not DGROUP' % (ref['name'], ref['site']))
        elif ref['linked'] is None:
            problems.append('(b) %s reference shape %s at %d unsupported' % (ref['name'], ref['loc'], ref['site']))
        else:
            item = next((i for i in items if i['name'] == ref['name']), None)
            if item is not None and ref['linked'] != item['address']:
                problems.append('(b) %s operand at %d links to %d, claimed %d' % (ref['name'], ref['site'],
                                                                                ref['linked'], item['address']))
    # (d) link-dependent size bases
    for item in items:
        basis = item['size_basis']
        refs = by_name.get(item['name'], [])
        if basis['kind'] == 'neutral-unreferenced' and refs:
            problems.append('(d) neutral communal %s has %d references' % (item['name'], len(refs)))
        if basis['kind'] == 'absorbed-gap':
            problems += _indexed_witnesses(item, refs, image, dgroup_base)
    return problems


def _indexed_witnesses(item, refs, image, dgroup_base):
    """Each witness is an original instruction whose DGROUP displacement is a
    linked reference of this array and that indexes it with a register."""
    from data_extent_generator import _decoder
    decoder, capstone = _decoder()
    sites = {r['site'] for r in refs if r['site'] is not None}
    problems = []
    for at in item['size_basis'].get('indexed_sites', []):
        ins = next(iter(decoder.disasm(image[at:at + 8], at)), None) if type(at) is int else None
        mem = [op for op in (ins.operands if ins else []) if op.type == capstone.x86.X86_OP_MEM]
        ok = (ins is not None and len(mem) == 1 and ins.disp_size == 2 and
              at + ins.disp_offset in sites and (mem[0].mem.base or mem[0].mem.index) and
              ins.reg_name(mem[0].mem.base) != 'bp' and
              item['address'] <= dgroup_base + (mem[0].mem.disp & 0xFFFF) < item['address'] + item['size'])
        if not ok:
            problems.append('(d) %s indexed-access witness at %s is not a register-indexed reference'
                            % (item['name'], at))
    return problems


def read_map(text):
    """MAP publics {name: address} and segments [{start, stop, length, name, class}]."""
    segs = [{'start': int(m[1], 16), 'stop': int(m[2], 16), 'length': int(m[3], 16), 'name': m[4], 'class': m[5]}
            for m in re.finditer(r'^\s*([0-9A-F]{5})H\s+([0-9A-F]{5})H\s+([0-9A-F]{5})H\s+(\S+)\s+(\S+)', text, re.M)]
    pubs = {}
    part = text.split('Publics by Name')[1].split('Publics by Value')[0] if 'Publics by Name' in text else text
    for m in re.finditer(r'^\s*([0-9A-F]{4}):([0-9A-F]{4})\s+(?:Abs\s+|Res\s+|Imp\s+)?(\S+)\s*$', part, re.M):
        pubs[m[3]] = int(m[1], 16) * 16 + int(m[2], 16)
    return pubs, segs


def gate_result(problems):
    groups = {k: [p for p in problems if p.startswith('(%s)' % k)] for k in 'abcd'}
    return {'status': 'PLACED' if not problems else 'REFUSED',
            'conditions': {k: ('PASS' if not v else 'FAIL') for k, v in groups.items()},
            'problems': problems}


# --------------------------------------------------------------------------- promotion
def load_candidate(path):
    """A communal-unit candidate (batch line `COMMUNAL PATH`): {schema, start,
    end, communals}; its id and kind are fixed."""
    spec = read_json(Path(path))
    require(isinstance(spec, dict) and set(spec) == {'schema', 'start', 'end', 'communals'} and
            spec['schema'] == SCHEMA, 'Communal unit candidate form differs')
    return {'id': SEGMENT, 'kind': KIND, 'classification': KIND, 'placement': PLACEMENT, 'segment': SEGMENT,
            'schema': SCHEMA, 'start': spec['start'], 'end': spec['end'], 'communals': spec['communals']}


def attach(manifest, row):
    """The manifest with the raw communal unit replaced exactly by `row`."""
    import copy
    result = copy.deepcopy(manifest)
    check_row_form(row)
    rows = result['bss_owners']
    raw = [i for i, o in enumerate(rows) if o.get('raw_form') == 'communal-unit' and o['kind'] == 'UNRESOLVED_RAW']
    require(len(raw) == 1 and raw[0] == len(rows) - 1 and
            (rows[raw[0]]['start'], rows[raw[0]]['end']) == (row['start'], row['end']),
            'Communal unit candidate does not replace the raw communal unit exactly')
    rows[raw[0]] = copy.deepcopy(row)
    return result


# --------------------------------------------------------------------------- dry run on a link directory
def references_from_link_dir(link_dir, names, linked_image, dgroup_base, listing_dir=None):
    """Linked objects of a reallink-style link directory (M*.OBJ; the unit listing
    objects.json of that directory or of `listing_dir`, the inputs it was staged from)."""
    import json
    from omf import OmfReader
    listing_path = Path(link_dir) / 'objects.json'
    if not listing_path.exists() and listing_dir is not None:
        listing_path = Path(listing_dir) / 'objects.json'
    listing = json.loads(listing_path.read_text())
    objects, declared = [], {}
    for row in listing:
        blob = (Path(link_dir) / row['file']).read_bytes()
        obj = OmfReader(communals=True).read(blob)
        place = {}
        for s in obj.segment_defs:
            if not s['length']:
                continue
            if s['class'] in ('CODE', 'FAR_DATA') or (row['kind'] == 'data-module' and s['name'] == '_DATA'):
                place[s['name']] = row['start']
            for seg, a, _ in row.get('data') or []:
                if seg == s['name']:
                    place[seg] = a
        objects.append({'id': row['unit'], 'obj': obj, 'place': place, 'file': row['file'], 'row': row})
        for c in obj.communals:
            declared.setdefault(c['name'], []).append(
                (row['unit'], c['length'], row['kind'] not in ('raw-code', 'raw-data', 'bss', 'prelude', 'stack')))
    return objects, declared, linked_references(objects, names, linked_image, dgroup_base)


def dry_run(link_dir, candidate=None, base_dir=None, manifest=None):
    """Evaluate (a)-(d) on an existing link directory (e.g. the commfit staged
    set) WITHOUT publishing.  Rows come from `candidate` or, when absent, from
    the MAP of that link (every public inside the raw communal extent, sizes to
    the next start, basis unknown).  base_dir: the canonical link inputs; an
    object differing from its canonical compile is not an exact declarer."""
    import json
    from common import sha
    from mz import MZ
    link_dir = Path(link_dir)
    layout = read_json(ROOT / 'layout/data-symbols.json')
    base = layout['frame_load_address']
    text = (link_dir / 'RESULT.MAP').read_text(errors='replace')
    pubs, segs = read_map(text)
    exe = (link_dir / 'RESULT.EXE').read_bytes()
    linked = MZ.parse(exe).load_image(exe)
    oracle = (ROOT / 'build/oracle/load-image.bin').read_bytes()
    manifest = manifest or read_json(ROOT / 'layout/manifest.json')
    if candidate is not None:
        row = load_candidate(candidate)
    else:
        seg = [s for s in segs if s['name'] == SEGMENT]
        require(len(seg) == 1, 'No c_common segment in the MAP')
        lo, hi = seg[0]['start'], seg[0]['stop'] + 1
        inside = sorted((a, n) for n, a in pubs.items() if lo <= a < hi)
        items = []
        for k, (a, n) in enumerate(inside):
            nxt = inside[k + 1][0] if k + 1 < len(inside) else hi
            items.append({'name': n, 'address': a, 'size': nxt - a, 'declarers': ['?'],
                          'size_basis': {'kind': 'unknown-next-start'}})
        row = {'id': SEGMENT, 'kind': KIND, 'classification': KIND, 'placement': PLACEMENT, 'segment': SEGMENT,
               'schema': SCHEMA, 'start': lo, 'end': hi, 'communals': items}
    items = row['communals']
    names = {i['name'] for i in items}
    objects, declared, refs = references_from_link_dir(link_dir, names, linked, base, base_dir)
    exact = None
    if base_dir is not None:
        # An exact declarer is the canonical link input of its unit, or (pinned
        # runtime data members) the hash-pinned member itself, COMDEFs included.
        canon = {r['file']: r['sha256'] for r in json.loads((Path(base_dir) / 'objects.json').read_text())}
        import runtime_binding
        pinned = {'rtd_' + m['id']: m['module_sha256'] for m in runtime_binding.runtime_data_members(manifest)}
        exact = {}
        for o in objects:
            digest = sha((link_dir / o['file']).read_bytes())
            same = digest == canon.get(o['file']) or digest == pinned.get(o['id'])
            exact[o['id']] = exact.get(o['id'], True) and same
    # declarers per item re-derived (candidate rows may name them; derived ones are reported)
    derived = {n: sorted({u for u, _, _ in rows}) for n, rows in declared.items()}
    if candidate is None:
        for i in items:
            i['declarers'] = derived.get(i['name'], [])
            if i['declarers'] and all(u.startswith('rtd_') for u in i['declarers']):
                i['size_basis'] = {'kind': 'pinned-member'}
    problems = []
    try:
        check_row_form(row, layout)
    except ValueError as error:
        problems.append('(d) row form: %s' % error)
    problems += check_link(items, declared, refs, pubs, segs, oracle, base, exact)
    edata = layout['bss_start']
    padded = linked + bytes(max(0, edata - len(linked)))     # LINK leaves the zero gap before _edata unwritten
    image_equal = padded[:edata] == oracle[:edata] and not any(oracle[len(linked):edata])
    if not image_equal:
        # MAP rows alone are self-consistent; the image decides the targets.
        problems.append('(b) linked load image differs from the oracle')
    problems += check_names(items, manifest)
    for i in items:
        if i['size_basis'].get('kind') not in SIZE_BASES:
            problems.append('(d) %s size basis %s is not grounded' % (i['name'], i['size_basis'].get('kind')))
    result = gate_result(problems)
    counts = {}
    for p in problems:
        key = re.sub(r'[0-9]+', 'N', re.sub(r'_[A-Za-z0-9_$@?]+', '_X', p))[:90]
        counts[key] = counts.get(key, 0) + 1
    result.update({'link_dir': str(link_dir), 'communals': len(items), 'references': len(refs),
                   'image_equal': image_equal,
                   'map_c_common': [s for s in segs if s['name'] == SEGMENT],
                   'map_exact': sum(1 for i in items if pubs.get(i['name']) == i['address']),
                   'operands_exact': sum(1 for r in refs if r['linked'] is not None and
                                         any(r['linked'] == i['address'] for i in items if i['name'] == r['name'])),
                   'problem_classes': dict(sorted(counts.items(), key=lambda t: -t[1])),
                   'problems': problems[:60]})
    return result


# --------------------------------------------------------------------------- pinned-tool fixture
FIXTURE_ASM = r'''DGROUP GROUP _DATA, _BSS
PUBLIC __acrtused
__acrtused EQU 9876h
COMM NEAR _zeta_buf:BYTE:300
COMM NEAR _asm_comm:BYTE:6
COMM NEAR _shared_tab:WORD:4
COMM NEAR _one_byte:BYTE
COMM NEAR _mid_word:WORD
A_TEXT SEGMENT BYTE PUBLIC 'CODE'
 ASSUME CS:A_TEXT, DS:DGROUP
start PROC FAR
 mov ax, word ptr _asm_comm
 mov ax, word ptr _shared_tab+2
 mov ax, word ptr _c_int
 mov al, _one_byte
 mov ax, _mid_word
 mov al, _zeta_buf+299
 ret
start ENDP
A_TEXT ENDS
_DATA SEGMENT WORD PUBLIC 'DATA'
 dw 1
_DATA ENDS
_BSS SEGMENT WORD PUBLIC 'BSS'
 db 5 dup(?)
_BSS ENDS
EXTRN _c_int:WORD
STACK SEGMENT PARA STACK 'STACK'
 db 64 dup(0)
STACK ENDS
END start
'''
FIXTURE_C = {
    'C1': b'int c_int;\nchar c_arr[7];\nint shared_tab[4];\n'
          b'int use1(void) { return c_int + c_arr[3] + shared_tab[1]; }\n',
    'C2': b'extern char asm_comm[];\nint c_int;\nlong c_long;\n'
          b'int use2(void) { return asm_comm[1] + c_int + (int)c_long; }\n',
}


def comm_near_fixture(work=None):
    """Pinned MASM 5.10 / MSC 5.10 / LINK 3.65 fixture: MASM `COMM NEAR` and MSC
    tentative definitions, one name declared by both, linked after an odd-ended
    `_BSS`.  Returns the COMDEF records, MAP rows and every communal reference
    read back from the linked image (test input only; never image ownership)."""
    import tempfile
    import link_order_probe as L
    from compiler import compile_source, CompileFailure
    from omf import OmfReader
    from mz import MZ
    work = Path(work or tempfile.mkdtemp(prefix='cf', dir=ROOT / 'build/probes'))
    work.mkdir(parents=True, exist_ok=True)
    blobs = {'A': L._assemble(work, 'A', FIXTURE_ASM)}
    for name, source in FIXTURE_C.items():
        try:
            obj, receipt = compile_source(source, 'msc510-medium', communals=True)
            directory = Path(receipt['work_directory'])
        except CompileFailure as error:
            raise AssertionError('fixture C compile failed: %s' % error)
        blobs[name] = (directory / 'UNIT.OBJ').read_bytes()
        (work / (name + '.OBJ')).write_bytes(blobs[name])

    def comdef_records(blob):
        out, at = [], 0
        while at < len(blob):
            kind, length = blob[at], struct.unpack_from('<H', blob, at + 1)[0]
            if kind == 0xB0:
                out.append(blob[at + 3:at + 2 + length].hex())
            at += 3 + length
        return out
    mz, mapping, log = L._link(work, ['A.OBJ', 'C1.OBJ', 'C2.OBJ'], 'msc510-medium')
    exe = (work / 'RESULT.EXE').read_bytes()
    image = mz.load_image(exe)
    pubs, segs = read_map(mapping)
    origin = re.search(r'^\s*([0-9A-F]{4}):([0-9A-F])\s+DGROUP\s*$', mapping, re.M)
    dgroup = int(origin[1], 16) * 16 + int(origin[2], 16)          # the DGROUP group frame
    objects, declared, at = [], {}, {}
    for name, blob in blobs.items():                  # link order A, C1, C2
        obj = OmfReader(communals=True).read(blob)
        seg = next(s for s in obj.segment_defs if s['class'] == 'CODE' and s['length'])
        code, step = seg['name'], {'byte': 1, 'word': 2, 'paragraph': 16}[seg['alignment']]
        start = at.get(code, next(s for s in segs if s['name'] == code)['start'])
        start = -(-start // step) * step
        at[code] = start + seg['length']
        place = {code: start}
        objects.append({'id': name, 'obj': obj, 'place': place})
        for c in obj.communals:
            declared.setdefault(c['name'], []).append((name, c['length'], True))
    names = set(declared)
    refs = linked_references(objects, names, image, dgroup)
    return {'comdef_records': {n: comdef_record for n, comdef_record in
                               ((n, comdef_records(b)) for n, b in blobs.items())},
            'communals': {n: [(c['name'], c['kind'], c['length'])
                              for c in OmfReader(communals=True).read(b).communals] for n, b in blobs.items()},
            'segments': segs, 'publics': pubs, 'dgroup': dgroup, 'declared': declared,
            'references': refs, 'image': image, 'link_log': log, 'work': str(work)}


def main():
    import argparse
    import json
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    d = sub.add_parser('dry-run', help='evaluate (a)-(d) on an existing link directory; publishes nothing')
    d.add_argument('link_dir')
    d.add_argument('--candidate', help='communal-unit candidate JSON (default: rows read from the MAP)')
    d.add_argument('--base-dir', help='canonical link inputs (objects.json) to judge declarer exactness')
    d.add_argument('--out')
    sub.add_parser('fixture', help='run the pinned-tool COMM NEAR / tentative-definition fixture')
    a = ap.parse_args()
    if a.cmd == 'dry-run':
        result = dry_run(a.link_dir, a.candidate, a.base_dir)
        text = json.dumps(result, indent=1, default=str)
        if a.out:
            Path(a.out).write_text(text)
        print(json.dumps({k: v for k, v in result.items() if k != 'problems'}, indent=1, default=str))
    else:
        f = comm_near_fixture()
        print(json.dumps({k: v for k, v in f.items() if k not in ('image',)}, indent=1, default=str)[:6000])


if __name__ == '__main__':
    main()
