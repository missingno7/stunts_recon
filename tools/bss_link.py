"""BSS storage ownership placed by the real LINK 3.65 run (integ32).

Two ownership kinds for zero-initialised DGROUP storage.  Neither is placed by
this module: the only placement authority is one run of the pinned LINK
(tools/reallink.py); this module states the rules, reads the link result and
decides whether every claimed address was reproduced.

(a) `link-module-order-v1` -- file-scope static storage in an object's own
    `_BSS` SEGDEF.  LINK concatenates the `_BSS` contributions in module
    processing order (each WORD aligned for MSC); the complete SEGDEF length is
    the object's extent.  A claim is proven only when the real link places the
    object's `_BSS` at the owner start (read back from the linked code operands
    of the object's own `_BSS` FIXUPPs, the MAP `_BSS` segment start and the
    whole-image equality).  integ34: the claimed start must also follow
    WITHOUT the link's (oracle-derived) processing order, from the image-
    derived code segment order (`independent_static_order`, basis
    image-code-segment-order-v1).  integ35: raw BSS debt is no longer one
    trailing synthetic unit.  Each object whose `_BSS` is not accepted keeps an
    explicit raw placeholder of its complete extent, linked right after that
    object (like raw code), so accepted storage is placed after raw
    predecessors; oracle sizes are used only for those raw placeholders.

(b) `link-communal-v1` -- near communals (OMF COMDEF type 62h).  LINK 3.65
    allocates them in `c_common` after the CRT0DAT XOE marker in its symbol
    table order.  `communal_order` (tools/communal_order.py, integ35) is the
    reconciled L7/L9 behavioural model (8-bit weighted name hash, 256 buckets
    ascending, head insertion on first sight); it is DIAGNOSTIC ONLY, like the
    MSC static-order model `static_order` (tools/msc_static_model.py).  A
    communal claim is proven only by the MAP address of its public name in a
    real link that reproduces the image, and a real link can only do that once
    every communal before it is real (the raw c_common unit has no names).  Production still refuses COMDEF-bearing
    objects (tools/omf.py) until such a link exists.
"""
from common import require

STATIC = 'link-module-order-v1'
COMMUNAL = 'link-communal-v1'
RUNTIME = 'link-runtime-member-v1'     # integ36: a pinned runtime member's own _BSS
PLACEMENTS = (STATIC, COMMUNAL)


# --------------------------------------------------------------------------- OMF COMDEF
def numeric_leaf(body, at):
    """OMF COMDEF numeric field: one byte < 80h, else 81h/84h/88h + 2/3/4 bytes."""
    first = body[at]
    if first < 0x80:
        return first, at + 1
    width = {0x81: 2, 0x84: 3, 0x88: 4}.get(first)
    require(width is not None and at + 1 + width <= len(body),
            'Unsupported OMF COMDEF numeric leaf %02x' % first)
    return int.from_bytes(body[at + 1:at + 1 + width], 'little'), at + 1 + width


def parse_comdef(body):
    """Complete COMDEF record body (without type/length/checksum) -> rows.

    62h (near): one size field (bytes).  61h (far): element count then element
    size.  Corrects the L5-bss scratch scanner, which read one byte after 81h
    and exchanged the near/far classes (L6-comdef)."""
    rows, at = [], 0
    while at < len(body):
        n = body[at]
        name = body[at + 1:at + 1 + n].decode('ascii')
        at += 1 + n
        type_index = body[at]
        require(type_index < 0x80, 'Unsupported COMDEF type index')
        at += 1
        kind = body[at]
        at += 1
        if kind == 0x62:
            size, at = numeric_leaf(body, at)
            rows.append({'name': name, 'kind': 'near', 'size': size, 'type_index': type_index})
        elif kind == 0x61:
            count, at = numeric_leaf(body, at)
            element, at = numeric_leaf(body, at)
            rows.append({'name': name, 'kind': 'far', 'size': count * element, 'count': count,
                         'element': element, 'type_index': type_index})
        else:
            require(False, 'Unsupported COMDEF data type %02x' % kind)
    return rows


# --------------------------------------------------------------------------- diagnostic models (integ35)
# Both models live in their own documented DIAGNOSTIC modules with fixture
# regressions (tools/communal_order.py, tools/msc_static_model.py).  They are
# design aids for choosing names and never decide a placement: statics are
# proven by the compiler's OMF plus the real link, communals by the MAP of a
# real link that reproduces the image.
def name_hash(name):
    """LINK 3.65 16-bit weighted name sum (communal_order.name_hash)."""
    import communal_order as model
    return model.name_hash(name)


def communal_order(commons, others=(), near_base=0, far_base=0):
    """Diagnostic LINK 3.65 communal walk (communal_order.order): buckets
    (hash & 255) ascending, each chain head-first; a symbol node is
    head-inserted when first seen (a prior EXTDEF/PUBDEF counts); duplicate
    COMDEFs merge to the larger size; an even-sized common starts even.
    `commons`: dicts name/size/kind/encounter; `others`: dicts name/encounter.
    Never a placement proof."""
    import communal_order as model
    rows = model.order([{'name': r['name'], 'size': r['size'], 'kind': r.get('kind', 'near'),
                         'encounter': r['encounter']} for r in commons],
                       [{'name': o['name'], 'encounter': o['encounter']} for o in others],
                       near_base=near_base, far_base=far_base)
    return [{'name': r['name'], 'kind': r['kind'], 'address': r['address'], 'size': r['size'],
             'bucket': r['bucket']} for r in rows]


def static_order(names, sizes, types):
    """Diagnostic MSC 5.10 file-static `_BSS` offsets (msc_static_model.order):
    {name: offset} for identifiers in declaration order.  Never a placement
    proof: the compiled object's own `_BSS` FIXUPPs and the real link decide."""
    import msc_static_model as model
    return model.order(names, sizes, types)


def static_layout(items):
    """Diagnostic MSC 5.10 translation-unit `_BSS` layout (msc_static_model.layout,
    integ37 flush rule): ({name: offset}, length).  Never a placement proof."""
    import msc_static_model as model
    return model.layout(items)


def static_order_residual(names, sizes, types, observed):
    """Diagnostic: statics whose modelled offset differs from `observed`
    ({name: offset}, e.g. the target placement a candidate must reproduce)."""
    predicted = static_order(names, sizes, types)
    return {n: (predicted.get(n), observed[n]) for n in observed if predicted.get(n) != observed[n]}


# --------------------------------------------------------------------------- real-link checks
def accepted_rows(manifest):
    return [o for o in manifest.get('bss_owners', []) if o['kind'] != 'UNRESOLVED_RAW']


def raw_rows(manifest):
    return [o for o in manifest.get('bss_owners', []) if o['kind'] == 'UNRESOLVED_RAW']


# --------------------------------------------------------------------------- raw BSS placeholders (integ35)
# Raw BSS debt occupies its own place in link order, like raw code: every
# object whose `_BSS` is not accepted keeps one UNRESOLVED_RAW placeholder of
# its complete extent (`object-bss`), linked as a raw `_BSS` contribution right
# after that object in processing order.  Placeholder extents are oracle-sized
# raw debt (evidence/bss-partition.json) and never size or place accepted
# storage.  A `link-word-fill` byte is re-created by LINK from the next
# contribution's WORD alignment; [XOE,_end) is one raw `communal-unit`
# (LINK's c_common) until the communals are reconstructed.
OBJECT_BSS, WORD_FILL, COMMUNAL_UNIT = 'object-bss', 'link-word-fill', 'communal-unit'
RAW_FORMS = (OBJECT_BSS, WORD_FILL, COMMUNAL_UNIT)
PARTITION_EVIDENCE = 'evidence/bss-partition.json'
# integ39: LINK makes its c_common segment PARAGRAPH aligned (pinned LINK 3.65
# fixture tests/fixtures/masm510_comm_near_fixture.json; the commfit staged
# link of the real program).  The bytes between the end of the complete BSS
# class sections before it (the CRT0DAT XOE marker) and the next paragraph are
# LINK fill, owned by one LINK_FILL row (communal_unit.checked_communal_fill);
# the raw communal unit starts at that paragraph.  The whole c_common unit is
# accepted at once as one LINK_COMMUNAL row (tools/communal_unit.py).
COMMUNAL_FILL_BASIS = 'link-communal-paragraph-v1'
COMMUNAL_KIND = 'LINK_COMMUNAL'


def _row_object(row, objects, owners):
    """objmap object id of one bss_owners row (raw: recorded; accepted: its parent's)."""
    if row['kind'] == 'UNRESOLVED_RAW':
        return row.get('object')
    parent = owners.get(row.get('parent'))
    if parent is None:
        return None
    hits = [o['id'] for o in objects if o['start'] <= parent['start'] < o['end']]
    return hits[0] if len(hits) == 1 else None


def check_partition(manifest, objects, evidence, bss_start, bss_end):
    """Structural proof of the BSS ownership partition (no link run).

    Returns rows [{id, kind, form, object, start, end}] in address order; raises
    on any violation.  Every raw row is one reviewed evidence row; an accepted
    row replaces exactly one object's placeholder (same object and extent) and
    no object has two BSS rows."""
    rows = manifest.get('bss_owners')
    require(isinstance(rows, list) and rows, 'BSS ownership partition missing')
    owners = {o['id']: o for o in manifest['owners']}
    by_id = {e['id']: e for e in evidence['rows']}
    require(evidence.get('schema') == 'bss-raw-placeholders-v1' and len(by_id) == len(evidence['rows']),
            'BSS placeholder evidence differs')
    out, position, seen_objects, used = [], bss_start, set(), set()
    for i, row in enumerate(rows):
        require(row['start'] == position and row['start'] < row['end'] <= bss_end, 'BSS ownership gap/overlap')
        position = row['end']
        obj = _row_object(row, objects, owners)
        if row['kind'] == 'LINK_FILL':
            # integ39: the c_common paragraph fill; it is re-derived from its
            # neighbours (communal_unit.checked_communal_fill), never from evidence.
            form = row.get('basis')
            nxt = rows[i + 1] if i + 1 < len(rows) else {}
            require(form == COMMUNAL_FILL_BASIS and 0 < row['end'] - row['start'] < 16 and
                    row['end'] % 16 == 0 and row['end'] == -(-row['start'] // 16) * 16 and
                    nxt.get('start') == row['end'] and
                    (nxt.get('raw_form') == COMMUNAL_UNIT or nxt.get('kind') == COMMUNAL_KIND),
                    'BSS LINK fill is not the paragraph fill before c_common: %s' % row['id'])
            out.append({'id': row['id'], 'kind': row['kind'], 'form': form, 'object': None,
                        'start': row['start'], 'end': row['end']})
            continue
        if row['kind'] == COMMUNAL_KIND:
            # integ39: the whole c_common unit accepted at once replaces the raw
            # communal unit placeholder exactly (communal_unit.check_row).
            ev = [e for e in evidence['rows'] if e['raw_form'] == COMMUNAL_UNIT]
            require(i == len(rows) - 1 and row['end'] == bss_end and len(ev) == 1 and
                    (ev[0]['start'], ev[0]['end']) == (row['start'], row['end']) and
                    row.get('placement') == COMMUNAL,
                    'Accepted communal unit does not replace the raw communal unit exactly')
            used.add(ev[0]['id'])
            out.append({'id': row['id'], 'kind': row['kind'], 'form': COMMUNAL_UNIT, 'object': None,
                        'start': row['start'], 'end': row['end']})
            continue
        if row['kind'] == 'UNRESOLVED_RAW':
            form = row.get('raw_form')
            ev = by_id.get(row['id'])
            require(form in RAW_FORMS and ev is not None and
                    (ev['raw_form'], ev['object'], ev['start'], ev['end']) ==
                    (form, row.get('object'), row['start'], row['end']),
                    'Raw BSS row is not a reviewed placeholder: %s' % row['id'])
            used.add(row['id'])
            if form == OBJECT_BSS:
                require(any(o['id'] == obj for o in objects), 'Raw BSS placeholder names no object')
            elif form == WORD_FILL:
                require(row['end'] - row['start'] == 1 and row['start'] & 1 and 0 < i < len(rows) - 1 and
                        not rows[i + 1]['start'] & 1, 'Raw BSS word fill is not one odd byte')
            else:
                require(i == len(rows) - 1 and row['end'] == bss_end, 'Raw communal unit is not the BSS tail')
        else:
            require(obj is not None, 'Accepted BSS row has no object')
            form = row.get('placement')
            ev = [e for e in evidence['rows'] if e['raw_form'] == OBJECT_BSS and e['object'] == obj]
            require(not ev or (ev[0]['start'], ev[0]['end']) == (row['start'], row['end']),
                    'Accepted BSS row does not replace its object placeholder exactly: %s' % row['id'])
            used.update(e['id'] for e in ev)
        if obj is not None:
            require(obj not in seen_objects, 'Object has two BSS rows: %s' % obj)
            seen_objects.add(obj)
        out.append({'id': row['id'], 'kind': row['kind'], 'form': form, 'object': obj,
                    'start': row['start'], 'end': row['end']})
    require(position == bss_end, 'BSS ownership does not cover clear range')
    require(used == set(by_id), 'BSS placeholder evidence rows without a partition row: %s'
            % sorted(set(by_id) - used))
    return out


def load_partition(manifest=None):
    """check_partition against the canonical objmap and evidence."""
    from common import ROOT, read_json
    manifest = manifest if manifest is not None else read_json(ROOT / 'layout/manifest.json')
    layout = read_json(ROOT / 'layout/data-symbols.json')
    return check_partition(manifest, read_json(ROOT / 'layout/link-objects.json')['objects'],
                           read_json(ROOT / PARTITION_EVIDENCE), layout['bss_start'], layout['bss_end'])


def ownership_report(rows):
    """Per-object BSS ownership (validation report): accepted storage, raw
    per-object placeholders (game / pinned runtime), fill and communal debt."""
    out = {'accepted': [], 'accepted_runtime': [], 'raw_objects': [], 'raw_runtime': [], 'raw_fill': 0,
           'raw_communal': 0, 'link_fill': 0, 'accepted_communal': 0}
    for r in rows:
        size = r['end'] - r['start']
        if r['kind'] == 'LINK_FILL':
            out['link_fill'] += size
        elif r['kind'] == COMMUNAL_KIND:
            out['accepted_communal'] += size
        elif r['kind'] != 'UNRESOLVED_RAW':
            out['accepted_runtime' if r['object'].startswith('rt_') else 'accepted'].append([r['object'], r['id'], size])
        elif r['form'] == OBJECT_BSS:
            out['raw_runtime' if r['object'].startswith('rt_') else 'raw_objects'].append([r['object'], size])
        elif r['form'] == WORD_FILL:
            out['raw_fill'] += size
        else:
            out['raw_communal'] += size
    out['bytes'] = {'accepted': sum(x[2] for x in out['accepted']),
                    'accepted_runtime': sum(x[2] for x in out['accepted_runtime']),
                    'raw_objects': sum(x[1] for x in out['raw_objects']),
                    'raw_runtime': sum(x[1] for x in out['raw_runtime']),
                    'raw_fill': out['raw_fill'], 'raw_communal': out['raw_communal'],
                    'link_fill': out['link_fill'], 'accepted_communal': out['accepted_communal']}
    return out


def grounded_member_bss(obj, code_start, image, dgroup_base):
    """integ35: the load address of a pinned runtime member's own `_BSS`, read
    from the image operands of every own-`_BSS` FIXUPP of its hash-pinned code
    (an image fact independent of link order); None unless unique."""
    refs = own_bss_addends(obj, '_TEXT')
    starts = {dgroup_base + int.from_bytes(image[code_start + o:code_start + o + 2], 'little') - a
              for o, a in refs}
    return (next(iter(starts)), len(refs)) if len(starts) == 1 else (None, len(refs))


def own_bss_addends(obj, code_segment):
    """(code offset, addend) of every CODE FIXUPP naming the object's own _BSS."""
    out = []
    for f in obj.linker_fixups:
        if f['target_kind'] == 'segment' and f['target'] == '_BSS':
            require(f['segment'] == code_segment and f['loc'] == 'offset16' and not f['self_relative'],
                    'Unsupported own _BSS FIXUPP shape')
            encoded = int.from_bytes(bytes.fromhex(f['encoded_addend']), 'little')
            displacement = f.get('displacement') or 0
            require(not (encoded and displacement), 'Ambiguous own _BSS addend')
            out.append((f['offset'], encoded + displacement))
    return out


ORDER_BASIS = 'image-code-segment-order-v1'
_ALIGN = {'byte': 1, 'word': 2, 'para': 16, 'paragraph': 16, 'page': 256}


def independent_static_order(rows, modules, code_segments, bss_start):
    """integ34: where each static owner's `_BSS` must start, derived WITHOUT the
    link's (oracle-derived) processing order.

    LINK facts: (1) with /DOSSEG the CODE-class segments are laid out in the
    order in which LINK first sees each segment name while it processes the
    modules; (2) the `_BSS` contributions are concatenated in module processing
    order.  Image fact: the load-image start of every code segment (the reviewed
    unit map, never the relocation table).  Hence a module that is the only
    declarer of a CODE segment laid out before the owner's (sole-declared)
    segment was processed before the owner; a module all of whose CODE SEGDEFs
    are laid out after it was processed after the owner; any other module that
    contributes `_BSS` (no CODE SEGDEF, an unplaced or shared earlier segment) is
    ambiguous and refuses the claim.  The predicted start is `_edata` plus the
    aligned `_BSS` of the modules proven earlier.

    modules: dicts id / code (every CODE SEGDEF name, any length) / bss (nonzero
    `_BSS` length) / bss_align / owners (static owner row ids).  The trailing
    raw communal unit is not a module here: it is LINK's c_common, after every
    `_BSS` contribution by class order.  code_segments: CODE segment names in
    load-image order.

    integ35: a raw `_BSS` placeholder module carries `host` (the module it is
    linked immediately after, i.e. its own object's code module) and takes its
    host's processing position; its oracle size is raw debt only.  A pinned
    runtime member's placeholder may carry `grounded` (its `_BSS` start read
    from the image operands of its hash-pinned code, `grounded_member_bss`):
    when the host position is undetermined (shared `_TEXT`), the grounded
    contribution is before or after the claim by address and must fit
    exactly at its turn in the predicted concatenation."""
    position = {name: i for i, name in enumerate(code_segments)}
    by_id = {m['id']: m for m in modules}
    declarers = {}
    for m in modules:
        if m.get('host'):
            continue
        for name in set(m['code']):
            declarers.setdefault(name, set()).add(m['id'])

    def align(at, m):
        step = _ALIGN.get(m.get('bss_align') or 'word', 2)
        return -(-at // step) * step

    out = {}
    for row in rows:
        problems, before, after, ambiguous, grounded = [], [], [], [], []
        holders = [m for m in modules if row['id'] in m.get('owners', ())]
        own = holders[0] if len(holders) == 1 else None
        segment = None
        if own is None:
            problems.append('owner module not unique in the link input')
        else:
            names = [n for n in own.get('code_nonempty', own['code'])]
            if len(set(names)) != 1:
                problems.append('owner module has %d nonempty CODE segments' % len(set(names)))
            else:
                segment = names[0]
                if segment not in position:
                    problems.append('owner CODE segment %s has no image position' % segment)
                elif declarers.get(segment) != {own['id']}:
                    problems.append('owner CODE segment %s is also declared by %s'
                                    % (segment, sorted(declarers.get(segment, set()) - {own['id']})))
        at = position.get(segment)

        def classify(m):
            if m.get('host'):
                host = by_id.get(m['host'])
                if host is None or host is own:
                    return None, None
                return classify(host)
            code = set(m['code'])
            if not code or any(n not in position for n in code):
                return None, None
            introduced = [n for n in code if position[n] < at and declarers[n] == {m['id']}]
            if introduced:
                return 'before', min(position[n] for n in introduced)
            if all(position[n] > at for n in code):
                return 'after', None
            return None, None
        if not problems:
            for m in modules:
                if m is own or not m.get('bss'):
                    continue
                side, key = classify(m)
                if side is None and m.get('grounded') is not None:
                    if m['grounded'] + m['bss'] <= row['start']:
                        grounded.append(m)
                        continue
                    if m['grounded'] >= row['end']:
                        side = 'after'
                if side == 'before':
                    before.append((key, len(before), m))
                elif side == 'after':
                    after.append(m['id'])
                else:
                    ambiguous.append(m['id'])
            if ambiguous:
                problems.append('_BSS contributors of undetermined processing order: %s' % sorted(ambiguous)[:6])
        predicted = bss_start
        sequence = []
        pending = sorted(grounded, key=lambda m: m['grounded'])

        def place_grounded(upto):
            nonlocal predicted
            while pending and align(predicted, pending[0]) == pending[0]['grounded'] and \
                    pending[0]['grounded'] < upto:
                m = pending.pop(0)
                predicted = m['grounded'] + m['bss']
                sequence.append(m['id'])
        for _, _, m in sorted(before, key=lambda t: t[:2]):
            place_grounded(align(predicted, m) + 1)
            predicted = align(predicted, m) + m['bss']
            sequence.append(m['id'])
        place_grounded(1 << 30)
        if pending:
            problems.append('grounded _BSS contribution(s) %s do not fit the image-derived order'
                            % [m['id'] for m in pending])
        if own is not None:
            predicted = align(predicted, own)
        if not problems and predicted != row['start']:
            problems.append('image-derived order places _BSS at %d, claimed %d' % (predicted, row['start']))
        out[row['id']] = {'basis': ORDER_BASIS, 'segment': segment, 'before': sequence,
                          'after': len(after), 'ambiguous': sorted(ambiguous),
                          'predicted_start': predicted, 'problems': problems}
    return out


def check_static(rows, located, linked_image, map_segments, bss_start, dgroup_base, order=None):
    """Decide `link-module-order-v1` rows from one real link.

    located: owner id -> (obj, code segment name, code load start); the code
    start is where the verified linked image holds that object's code.
    order: `independent_static_order` result; the placement must also follow
    from it (integ34: no oracle-derived processing order decides a claim)."""
    out, ok = [], True
    seg = [s for s in map_segments if s['name'] == '_BSS']
    map_start = seg[0]['start'] if len(seg) == 1 else None
    for row in rows:
        result = {'id': row['id'], 'start': row['start'], 'end': row['end'],
                  'placement': row.get('placement')}
        problems = []
        if row.get('placement') != STATIC:
            problems.append('not a link-module-order-v1 owner')
        entry = located.get(row['id'])
        if entry is None:
            problems.append('owning object not linked from its source')
        else:
            obj, code_segment, code_start = entry
            size = obj.segment_length('_BSS') if '_BSS' in obj.segment_lengths else 0
            if size != row['end'] - row['start']:
                problems.append('complete _BSS SEGDEF length %d differs from owner extent' % size)
            starts = set()
            refs = own_bss_addends(obj, code_segment)
            for offset, addend in refs:
                value = int.from_bytes(linked_image[code_start + offset:code_start + offset + 2], 'little')
                starts.add(dgroup_base + value - addend)
            result['linked_references'] = len(refs)
            result['linked_starts'] = sorted(starts)
            if not refs:
                problems.append('no own _BSS code reference places the contribution')
            elif starts != {row['start']}:
                problems.append('real link placed _BSS at %s' % sorted(starts))
        if map_start != bss_start:
            problems.append('MAP _BSS segment start %s differs from _edata %d' % (map_start, bss_start))
        evidence = (order or {}).get(row['id'])
        if evidence is None:
            problems.append('no image-derived processing-order evidence for the _BSS start')
        else:
            result['independent_order'] = evidence
            problems += evidence['problems']
        result['problems'] = problems
        result['placed'] = not problems
        ok = ok and not problems
        out.append(result)
    return ok, out


def check_runtime_bss(rows, located, image, linked_image, map_segments, bss_start, dgroup_base):
    """integ36: decide `link-runtime-member-v1` rows (a pinned runtime member's
    own `_BSS`, owned as pinned runtime data).  The start is grounded
    independently of link order by the image operands of every own-`_BSS`
    FIXUPP of the hash-pinned code (`grounded_member_bss`), and the real link
    must place the member's `_BSS` there: the linked operands of the same
    FIXUPPs, with the MAP `_BSS` start at `_edata` and whole-image equality."""
    out, ok = [], True
    seg = [s for s in map_segments if s['name'] == '_BSS']
    map_start = seg[0]['start'] if len(seg) == 1 else None
    for row in rows:
        problems = []
        result = {'id': row['id'], 'start': row['start'], 'end': row['end'], 'placement': row.get('placement')}
        if row.get('placement') != RUNTIME:
            problems.append('not a link-runtime-member-v1 owner')
        entry = located.get(row['id'])
        if entry is None:
            problems.append('pinned member not linked from its own OBJ')
        else:
            obj, code_segment, code_start = entry
            if obj.segment_lengths.get('_BSS', 0) != row['end'] - row['start']:
                problems.append('complete _BSS SEGDEF length differs from owner extent')
            grounded, count = grounded_member_bss(obj, code_start, image, dgroup_base)
            refs = own_bss_addends(obj, code_segment)
            linked = {dgroup_base + int.from_bytes(linked_image[code_start + o:code_start + o + 2], 'little') - a
                      for o, a in refs}
            result.update(grounded_start=grounded, own_references=count, linked_starts=sorted(linked))
            if grounded != row['start']:
                problems.append('own code operands ground _BSS at %s' % grounded)
            if not refs or linked != {row['start']}:
                problems.append('real link placed _BSS at %s' % sorted(linked))
        if map_start != bss_start:
            problems.append('MAP _BSS segment start %s differs from _edata %d' % (map_start, bss_start))
        result['problems'] = problems
        result['placed'] = not problems
        ok = ok and not problems
        out.append(result)
    return ok, out


def check_communals(rows, map_publics):
    """MAP public address of each communal name (the MAP reading only; integ39:
    the acceptance decision is communal_unit.check_link, conditions a-d)."""
    out, ok = [], True
    for row in rows:
        names = row.get('communals') or []
        problems = [] if names else ['no communal names recorded']
        for item in names:
            linked = map_publics.get(item['name'])
            if linked != item['address']:
                problems.append('%s linked at %s, claimed %d' % (item['name'], linked, item['address']))
        out.append({'id': row['id'], 'problems': problems, 'placed': not problems})
        ok = ok and not problems
    return ok, out


def gate(summary, owner_ids=None):
    """Acceptance decision from a reallink summary: the whole image, relocation
    set and bank order, header and packed file reproduce, and every BSS owner
    (or the listed ones) is placed by the link.  Diagnostic real-link fields
    are only a gate here, for BSS storage claims."""
    bss = summary.get('bss') or {}
    rows = bss.get('owners', [])
    header = summary.get('header') or {}
    linked, oracle = header.get('linked') or {}, header.get('oracle') or {}
    problems = []
    for key in ('image_equal', 'relocation_set_equal', 'bank_order_equal', 'packed_equal'):
        if summary.get(key) is not True:
            problems.append('real link %s is %s' % (key, summary.get(key)))
    # maxalloc: LINK /CP:1 writes the minimum and EXEPACK the packed value; the
    # packed-file equality above decides it (the oracle's unpacked MZ carries 1984).
    fields = ('cs', 'ip', 'ss', 'sp', 'minalloc', 'image_size', 'relocations')
    if not linked or any(linked.get(k) != oracle.get(k) for k in fields):
        problems.append('real link header differs')
    if summary.get('link_errors'):
        problems.append('LINK errors: %s' % summary['link_errors'][:3])
    if summary.get('alias_shims'):
        problems.append('real link needs %d alias shims' % summary['alias_shims'])
    problems += bss.get('problems', [])
    # integ36: every linked pinned runtime storage row must be placed there.
    problems += (summary.get('runtime') or {}).get('problems', [])
    if owner_ids is not None and set(owner_ids) - {r['id'] for r in rows}:
        problems.append('BSS owners absent from the real link: %s' % sorted(set(owner_ids) - {r['id'] for r in rows}))
    for r in rows:
        problems += ['%s: %s' % (r['id'], p) for p in r.get('problems', [])]
    return {'status': 'PLACED' if not problems else 'REFUSED', 'problems': problems,
            'owners': [r['id'] for r in rows]}


# --------------------------------------------------------------------------- promotion / validation gate
def claims(recipe):
    """Whether a candidate recipe claims BSS storage (static _BSS or, integ39,
    COMDEF declarations of the communal unit)."""
    return bool('_BSS' in (recipe.get('secondary_dgroup_segments') or {}) or recipe.get('communal_declarations'))


def staged_gate(entries, tag='bss-gate', communal=None):
    """Real LINK 3.65 run of the canonical state with `entries` ((name, source
    bytes, recipe) in publication order) composed as promotion composes them;
    every BSS owner of a claiming entry must be placed by that link.  integ39:
    `communal` is a LINK_COMMUNAL row composed after the entries (the whole
    communal unit); it must be placed too."""
    import reallink
    report = reallink.run(tag=tag, log=lambda *args: None, stage=list(entries), stage_communal=communal)
    summary = reallink.summary(report)
    owners = sorted('%s:_BSS' % recipe['id'] for _, _, recipe in entries
                    if '_BSS' in (recipe.get('secondary_dgroup_segments') or {}))
    if communal is not None:
        owners.append(communal['id'])
    decision = gate(summary, owners if entries else None)
    decision['report'] = summary.get('report')
    require(decision['status'] == 'PLACED',
            'BSS storage is not placed by the real link: ' + '; '.join(decision['problems'][:6]))
    return decision


def linked_runtime_storage(manifest):
    """integ36: whether any accepted runtime member has storage placed by the real link."""
    return any(r.get('ownership') == 'linked' for o in manifest['owners'] if o['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY'
               for r in o.get('binding', {}).get('storage', {}).values())


def canonical_gate():
    """Validation: every accepted BSS owner and every linked runtime storage row
    of the canonical manifest is placed by a fresh real link (None when neither
    exists)."""
    from common import ROOT, read_json
    manifest = read_json(ROOT / 'layout/manifest.json')
    if not accepted_rows(manifest) and not linked_runtime_storage(manifest):
        return None
    return staged_gate([], tag='bss-validate')


def runtime_gate(candidates, tag='runtime-gate'):
    """integ36: the staged real link of pinned runtime candidates, composed as
    promote_runtime composes them; every accepted BSS owner and every linked
    runtime storage row (canonical and staged) must be placed by that link."""
    import reallink
    report = reallink.run(tag=tag, log=lambda *args: None, stage_runtime=list(candidates))
    summary = reallink.summary(report)
    decision = gate(summary)
    decision['report'] = summary.get('report')
    decision['runtime_rows'] = len((summary.get('runtime') or {}).get('rows', []))
    require(decision['status'] == 'PLACED',
            'Runtime storage is not placed by the real link: ' + '; '.join(decision['problems'][:6]))
    return decision
