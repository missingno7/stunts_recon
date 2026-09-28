"""Incremental RECORD_CLOSED_EXACT prefix proof over a candidate's own OMF records.

Source of truth is the complete object the pinned compiler emits for the whole
candidate translation unit, placed so that its CODE offset zero (its first
public) is the object start. Record boundaries are the candidate's real CODE
LEDATA records; the candidate relocation stream is its FIXUPPs in emitted
order (records ascending, FIXUPP order within a record), never sorted,
rewritten, simulated, or taken from the oracle. The immutable oracle is only
compared against afterwards.

A CODE record belongs to the RECORD_CLOSED_EXACT prefix when it and every
earlier record are (i) naturally closed by the compiler (a later CODE record
of the same object follows it; the final record is never counted), (ii) byte
identical to the oracle after (iii) every FIXUPP in them is bound by the
existing binder rules, and (iv) the candidate-derived relocation stream of the
whole prefix equals the oracle's relocation subsequence for the same interval,
in order. Bytes of closed records may depend on later code (forward jumps,
near calls to later publics); they are compared with the oracle and the proof
is re-derived from a fresh compile of the whole candidate on every use, so a
suffix change that alters such a byte moves or invalidates the prefix.

States: CODEGEN_EXACT (bytes outside FIXUPP fields and relocation sites agree),
EXACT_OPEN_RECORD (bound bytes, fixups and order exact, but the record is the
final, open candidate record), RECORD_CLOSED_EXACT (inside the derived
prefix), ACCEPTED (owned by the manifest). Diagnostic outputs (cut simulator,
classifier, blockdiff, tubench) are never inputs to this proof.

Example (verify-only research; writes only the given outputs):
    python tools/prefix_proof.py build/workers/NAME/whole_tu.c --report OUT.json --recipe-out OUT.recipe.json --id prefix_NAME
"""
import argparse
import copy
import struct
from pathlib import Path
from common import ROOT, identity, read_json, require, sha
from cut_simulator import ordered_data_records

SCHEMA = 'record-closed-prefix-v1'
STATES = ('CODEGEN_EXACT', 'EXACT_OPEN_RECORD', 'RECORD_CLOSED_EXACT', 'ACCEPTED')
RECORD_STATES = ('RECORD_CLOSED_EXACT', 'EXACT_OPEN_RECORD', 'BOUND_EXACT_AFTER_GAP',
                 'CODEGEN_EXACT', 'MISMATCH')
CATEGORIES = ('OBJECT_START', 'OPEN_RECORD', 'PUBLIC_ENTRY', 'SYMBOL_BINDING', 'FIXUP_BINDING', 'TU_DATA',
              'BYTE_MISMATCH', 'RELOCATION_ORDER', 'UNSUPPORTED_RELOCATION')
REQUIRED_KEYS = {'id', 'stable_id', 'kind', 'profile', 'source', 'preprocessor_closure',
                 'object_segment', 'start', 'end', 'target', 'prefix_of_object',
                 'prefix_members', 'object_declarations', 'expected_fixups',
                 'expected_relocations'}
OPTIONAL_KEYS = {'external_binding', 'secondary_dgroup_segments', 'secondary_external_targets',
                 'original_frame_load_address', 'compiler_flags', 'compiler_flags_register',
                 'subsumed_owners', 'subsumed_data_owners', 'evidence', 'sparse_zero'}
META_KEYS = {'schema', 'object_start', 'records', 'record_ends', 'candidate_object_length',
             'ends_mid_function'}
VERIFIED = ('BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED', 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED')


class PrefixFailure(ValueError):
    def __init__(self, category, message):
        super().__init__(f'{category}: {message}')
        self.category, self.detail = category, message


def _step(category, function, *args, **kwargs):
    try:
        return function(*args, **kwargs)
    except PrefixFailure:
        raise
    except ValueError as error:
        raise PrefixFailure(category, str(error)) from error


def candidate_records(obj, segment):
    """The candidate's CODE LEDATA records with their own FIXUPPs, in emitted order."""
    data = getattr(obj, 'omf_bytes', None)
    require(isinstance(data, bytes) and data, 'Prefix proof needs the emitted OMF object')
    names = {d['index']: d['name'] for d in obj.segment_defs}
    fixes, at, code = list(obj.linker_fixups), 0, []
    # Reviewed debug segments (MSC 6 /Zi CodeView) and their FIXUPPs were
    # excluded by read_object; their records carry no image bytes.
    debug = {d['index'] for d in getattr(obj, 'debug_segments', [])}
    for row in ordered_data_records(data):
        if row['segment_index'] in debug:
            continue
        own = fixes[at:at+row['fixups']]
        at += row['fixups']
        name = names.get(row['segment_index'])
        require(name is not None, 'Data record names no SEGDEF')
        if row['kind'] != 0xA0:
            require(name != segment and not own, 'Iterated CODE data has no record-closed prefix rule')
            continue
        end = row['offset'] + row['size']
        require(len(own) == row['fixups'] and
                all(f['segment'] == name and row['offset'] <= f['offset'] and
                    f['offset'] + f['width'] <= end for f in own),
                'FIXUPP does not lie inside its preceding LEDATA record')
        if name == segment:
            code.append({'index': len(code), 'start': row['offset'], 'end': end, 'fixups': own})
    require(at == len(fixes), 'FIXUPP/LEDATA association is incomplete')
    length = obj.segment_length(segment)
    require(code and code[0]['start'] == 0 and
            all(a['end'] == b['start'] for a, b in zip(code, code[1:])) and
            code[-1]['end'] == length and len(obj.segment_bytes(segment)) == length,
            'Candidate CODE records are not one ascending contiguous cover of the object')
    return code


def relocation_stream(fixes, start):
    """LINK MZ entries generated by the given FIXUPPs, in exactly their order."""
    rows = []
    for fix in fixes:
        require(fix['loc'] in ('pointer32', 'base16', 'offset16', 'loader-offset16'),
                'Unsupported relocation-generating fixup form ' + str(fix['loc']))
        if fix['loc'] in ('pointer32', 'base16'):
            site = start + fix['offset'] + (2 if fix['loc'] == 'pointer32' else 0)
            rows.append({'segment': (site // 65536) * 4096, 'offset': site % 65536,
                         'load_offset': site})
    return rows


def oracle_slice(relocations, start, lo, hi):
    """Ordered oracle MZ entries inside [start+lo, start+hi); a record at the
    object start also claims a word straddling it (as the complete probe does)."""
    first = start + lo - (1 if lo == 0 else 0)
    return [r for r in relocations if first <= r['load_offset'] < start + hi]


def binding_mode(external):
    """Existing external binder mode selected by the fixup shapes alone."""
    if not external:
        return None
    far = any(f['loc'] == 'pointer32' for f in external)
    pointers = any(f['loc'] in ('base16', 'loader-offset16') for f in external)
    data = any(f['loc'] == 'offset16' for f in external)
    if pointers:
        return 'external-far-call-code-pointer-dgroup-offset16-v1'
    if far and data:
        return 'external-far-call-dgroup-offset16-v1'
    return 'external-far-call-v1' if far else 'external-dgroup-offset16-v1'


class OracleContext:
    """Independent facts from the locked image, inventory and symbol registries."""

    def __init__(self, image, relocations, start):
        from function_evidence import current_inventory, reviewed_functions
        self.image, self.relocations, self.start = image, relocations, start
        inventory = current_inventory(image)
        require(inventory['load_sha256'] == sha(image), 'Inventory/oracle identity differs')
        reviewed_functions(image)
        self.rows = [f for f in inventory['functions']
                     if type(f.get('start')) is int and type(f.get('end')) is int]

    def verified(self, f):
        return (f['status'] == 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' or
                f['status'] == 'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED' and
                bool(f.get('start_evidence')) and bool(f.get('end_evidence')) and
                sha(self.image[f['start']:f['end']]) == f['sha256'])

    def entry(self, public, address, local):
        rows = [f for f in self.rows if f['start'] == address]
        require(len(rows) == 1, f'Candidate public {public} at load {address} is not one inventory entry')
        f = rows[0]
        require(self.verified(f), f'Inventory entry {f["name"]} is not verified')
        from asm_module import registry_publics
        require((public == f['name'] and bool(f.get('local_symbol'))) if local else
                ((public == '_' + f['name'] or public in registry_publics(f['name'])) and
                 not f.get('local_symbol')),
                f'Candidate public {public} differs from inventory entry {f["name"]}')
        return f

    def object_start(self, public, local):
        """The candidate object start is its code frame start (one MSC C object
        per NAME_TEXT segment). LINK's frame is the paragraph floor of the
        segment start, so the start lies in the frame's first paragraph and no
        inventory row starts between the frame base and it; the bytes there
        belong to a row that began before the frame (another segment)."""
        f = self.entry(public, self.start, local)
        base = self.frame()
        require(base <= self.start < base + 16,
                'Object start is not the start of its original code frame')
        require(all(g['start'] < base for g in self.rows_in(base, self.start)) and
                not any(g.get('segment_paragraph') == base // 16 and g['start'] < self.start
                        for g in self.rows),
                'An inventory entry of the same code frame precedes the object start')
        return f

    def rows_in(self, lo, hi):
        return [f for f in self.rows if f['start'] < hi and lo < f['end']]

    def frame(self):
        """Frame of the start entry from original relocated words only: the
        inventory frame, else the reviewed far-code alias of that entry, whose
        original relocated CALL/JMP anchors are rechecked by the resolver."""
        f = self.entry_row(self.start)
        frames = set()
        if type(f.get('segment_paragraph')) is int:
            frames.add(f['segment_paragraph'] * 16)
        alias = '_' + f['name']
        from code_symbols import resolve_code_symbols
        if alias in read_json(ROOT/'layout/code-symbols.json')['symbols']:
            symbol = resolve_code_symbols({alias}, self.image, self.relocations)[alias]
            require(symbol['load_address'] == self.start, 'Start alias lands elsewhere')
            frames.add(symbol['frame_load_address'])
        require(len(frames) == 1 and min(frames) % 16 == 0,
                'Object start lacks one independently grounded code frame')
        return frames.pop()

    def entry_row(self, address):
        rows = [f for f in self.rows if f['start'] == address]
        require(len(rows) == 1, 'Object start is not one inventory entry')
        return rows[0]

    def resolve(self, sub):
        from code_symbols import resolve_recipe_symbols
        return resolve_recipe_symbols(sub, self.image, self.relocations)

    def data_specs(self, obj, own):
        """TU-owned DGROUP placement only where original operands agree (tubench rule)."""
        from data_symbols import checked_dgroup_layout
        layout = checked_dgroup_layout(self.image, self.relocations)
        frame = layout['frame_load_address']
        specs = {}
        for name in sorted({f['target'] for f in own}):
            bases = set()
            for f in own:
                if f['target'] != name:
                    continue
                at = self.start + f['offset']
                word = int.from_bytes(self.image[at:at+2], 'little')
                addend = int.from_bytes(bytes.fromhex(f['encoded_addend']), 'little')
                bases.add(word - addend)
            require(len(bases) == 1 and 0 <= min(bases) < 65536,
                    f'Original operands do not agree on one {name} base')
            base = bases.pop()
            size = obj.segment_length(name)
            require(type(size) is int and size > 0, 'TU-owned segment is empty')
            lo, hi = frame + base, frame + base + size
            target = identity(bytes(size)) if name == '_BSS' else identity(self.image[lo:hi])
            specs[name] = {'dgroup_offset': base, 'start': lo, 'end': hi, 'target': target,
                           'expected_relocations': [r for r in self.relocations
                                                    if lo <= r['load_offset'] < hi]}
        return specs


def secondary_targets(obj, specs):
    names = {}
    for f in obj.linker_fixups:
        if f['segment'] in specs and f['target_kind'] == 'external':
            kind = 'code' if f['loc'] in ('pointer32', 'base16', 'loader-offset16') else 'data'
            require(names.setdefault(f['target'], kind) == kind,
                    'Secondary data external has two target kinds')
    return {'code': sorted(n for n, k in names.items() if k == 'code'),
            'data': sorted(n for n, k in names.items() if k == 'data')}


def _publics(obj, segment):
    rows = sorted((p for p in obj.publics if p['segment'] == segment), key=lambda p: p['offset'])
    require(rows and rows[0]['offset'] == 0, 'Candidate object has no public at its start')
    require(len({p['offset'] for p in rows}) == len(rows) and
            len({p['name'] for p in rows}) == len(rows), 'Duplicate candidate public')
    return rows


def _check_publics(obj, segment, ctx, lo, hi, length):
    """Every candidate public in [lo,hi) is its verified inventory entry, and
    every inventory row starting there is a candidate public (the tiling)."""
    S = ctx.start
    rows = _publics(obj, segment)
    local = {p['name'] for p in obj.local_publics}
    offsets = [p['offset'] for p in rows] + [length]
    placed = {S + p['offset'] for p in rows}
    for index, p in enumerate(rows):
        if lo <= p['offset'] < hi:
            f = ctx.entry(p['name'], S + p['offset'], p['name'] in local)
            following = offsets[index + 1]
            if following <= hi:
                require(f['end'] == S + following,
                        f'Inventory extent of {f["name"]} differs from the candidate member')
    for f in ctx.rows_in(S + lo, S + hi):
        require(f['start'] in placed, f'Inventory entry {f["name"]} is not a candidate public')


def _first_public(obj, segment):
    return _publics(obj, segment)[0]['name']


def bind_range(obj, segment, end, fixes, ctx):
    """Bind the given FIXUPPs of CODE [0,end) with the existing binder rules."""
    from binder import bind_contribution
    from secondary_contribution import bind_secondary
    S, image = ctx.start, ctx.image
    code = obj.segment_bytes(segment)[:end]
    require(len(code) == end, 'Prefix exceeds candidate CODE')
    pubs = {p['name']: p['offset'] for p in obj.publics if p['segment'] == segment}
    local = {p['name'] for p in obj.local_publics}
    require(all(f['segment'] == segment and f['offset'] + f['width'] <= end for f in fixes),
            'Prefix FIXUPP outside the bound interval')
    internal = [f for f in fixes if f['target_kind'] == 'external' and f['target'] in pubs and
                f['self_relative']]
    own_data = [f for f in fixes if f['target_kind'] == 'segment' and f['target'] != segment]
    local_code = [f for f in fixes if f['target_kind'] == 'segment' and f['target'] == segment]
    # Far pointer words to an own public (MOV AX,offset / MOV DX,seg), integ26.
    own_pointer = [f for f in fixes if f['target_kind'] == 'external' and f['target'] in pubs and
                   not f['self_relative']]
    special = {id(f) for f in internal + own_data + local_code + own_pointer}
    external = [f for f in fixes if id(f) not in special]
    external_ids = {id(f) for f in external}
    stream = _step('UNSUPPORTED_RELOCATION', relocation_stream, fixes, S)
    _step('FIXUP_BINDING', require,
          not any(f['target_kind'] == 'external' and f['target'] in pubs for f in external) and
          all(f['target_kind'] == 'external' for f in external),
          'Unsupported prefix FIXUPP target (own public without near CALL, group or frame target)')
    mode = binding_mode(external)
    payload, receipt = bytearray(code), {'mode': None}
    if mode is not None:
        public0 = _first_public(obj, segment)
        keep = ({f['target'] for f in external} |
                {f['frame'] for f in external if f['frame_method'] == 2} |
                {'__acrtused', public0})
        view = copy.copy(obj)
        view.segments = {segment: bytes(code)}
        view.segment_lengths = {n: (end if n == segment else 0) for n in obj.segment_lengths}
        view.segment_defs = [{**d, 'length': end if d['name'] == segment else 0}
                             for d in obj.segment_defs]
        view.publics = [{'name': public0, 'segment': segment, 'offset': 0}]
        view.local_publics, view.local_externals = [], []
        view.externals = [n for n in obj.externals if n in keep]
        view.linker_fixups = []
        for f in fixes:
            if id(f) not in external_ids:
                continue
            row = dict(f)
            if f['target_kind'] == 'external':
                _step('FIXUP_BINDING', require,
                      1 <= f['target_index'] <= len(obj.externals) and
                      obj.externals[f['target_index'] - 1] == f['target'],
                      'Original external target index differs')
                row['target_index'] = view.externals.index(f['target']) + 1
            if f['frame_method'] == 2 and f['frame_kind'] == 'external':
                row['frame_index'] = view.externals.index(f['frame']) + 1
            view.linker_fixups.append(row)
        sub = {'id': public0.lstrip('_'), 'kind': 'c', 'start': S, 'end': S + end,
               'object_segment': segment, 'public': public0,
               'expected_fixups': view.linker_fixups,
               'expected_relocations': relocation_stream(view.linker_fixups, S),
               'binding': {'mode': mode, 'declarations': {
                   'segments': view.segment_defs, 'groups': view.groups,
                   'publics': view.publics, 'externals': view.externals}}}
        symbols = _step('SYMBOL_BINDING', ctx.resolve, sub)
        bound, receipt = _step('FIXUP_BINDING', bind_contribution, view, sub, symbols)
        payload = bytearray(bound)
        _step('RELOCATION_ORDER', require,
              receipt['generated_relocations'] == relocation_stream(view.linker_fixups, S),
              'Binder relocation stream differs from candidate FIXUPP order')
    rows = []
    for f in internal:
        at, target = f['offset'], f['target']
        _step('FIXUP_BINDING', require,
              f['loc'] == 'offset16' and f['width'] == 2 and f['encoded_addend'] == '0000' and
              f['displacement'] == 0 and f['target_method'] == 2 and
              (f['frame_method'], f['frame_kind'], f['frame'], f['frame_index']) ==
              (5, 'target', target, 0) and 1 <= at <= end - 2 and
              code[at - 1] in (0xe8, 0xe9) and payload[at:at + 2] == bytes(2),
              'Unsupported internal near CALL datum')
        offset = pubs[target]
        _step('PUBLIC_ENTRY', ctx.entry, target, S + offset, target in local)
        value = offset - (at + 2)
        _step('FIXUP_BINDING', require, -32768 <= value <= 32767, 'Internal displacement overflow')
        struct.pack_into('<h', payload, at, value)
        rows.append({'offset': at, 'target': target, 'target_offset': offset,
                     'displacement': value, 'beyond_bound_interval': offset >= end})
    pointers = {}
    if own_pointer:
        frame = _step('FIXUP_BINDING', ctx.frame)
        length = obj.segment_length(segment)
        _step('FIXUP_BINDING', require,
              type(frame) is int and frame % 16 == 0 and frame <= S and S + length <= frame + 65536,
              'Own-public pointer frame does not hold the complete candidate object')
        for f in own_pointer:
            at = f['offset']
            _step('FIXUP_BINDING', require,
                  f['loc'] in ('loader-offset16', 'offset16', 'base16') and f['width'] == 2 and
                  f['target_method'] == 2 and f['displacement'] == 0 and
                  f['encoded_addend'] == '0000' and
                  (f['frame_method'], f['frame_kind'], f['frame'], f['frame_index']) ==
                  (5, 'target', f['target'], 0) and 1 <= at <= end - 2 and
                  0xb8 <= code[at - 1] <= 0xbf and payload[at:at + 2] == bytes(2),
                  'Unsupported own-public pointer datum')
            _step('PUBLIC_ENTRY', ctx.entry, f['target'], S + pubs[f['target']], f['target'] in local)
            pointers.setdefault(f['target'], {'offset': [], 'base': []})[
                'base' if f['loc'] == 'base16' else 'offset'].append(at)
            value = frame // 16 if f['loc'] == 'base16' else S - frame + pubs[f['target']]
            _step('FIXUP_BINDING', require, 0 <= value <= 65535, 'Own-public pointer overflow')
            struct.pack_into('<H', payload, at, value)
        _step('FIXUP_BINDING', require,
              all(v['base'] and sorted(x - 3 for x in v['base']) == sorted(v['offset'])
                  for v in pointers.values()),
              'Own-public pointer words are not adjacent MOV offset/segment pairs')
    tables = []
    if local_code:
        # Same rule as a complete single C module's switch-table words
        # (binder.bind_composed): offset16, own segment, frame F0/F5, zero FIXUPP
        # displacement, in-segment offset stored in the LEDATA word, one 64 KiB
        # frame. The module here is the complete candidate object; a target
        # after the prefix is a suffix dependency compared through the oracle.
        frame = _step('FIXUP_BINDING', ctx.frame)
        length = obj.segment_length(segment)
        definition, = [d for d in obj.segment_defs if d['name'] == segment]
        _step('FIXUP_BINDING', require,
              type(frame) is int and frame % 16 == 0 and frame <= S and S + length <= frame + 65536,
              'Own code-table frame does not hold the complete candidate object')
        for f in local_code:
            at = f['offset']
            target = int.from_bytes(bytes.fromhex(f['encoded_addend']), 'little')
            _step('FIXUP_BINDING', require,
                  f['loc'] == 'offset16' and f['width'] == 2 and not f['self_relative'] and
                  f['displacement'] == 0 and f['target_method'] == 0 and
                  f['target_index'] == definition['index'] and
                  (f['frame_method'], f['frame_kind'], f['frame'], f['frame_index']) in
                  ((0, 'segment', segment, definition['index']), (5, 'target', segment, 0)) and
                  0 <= at <= end - 2 and payload[at:at + 2] == code[at:at + 2] and
                  0 <= target < length,
                  'Unsupported own-segment code offset datum')
            value = S - frame + target
            _step('FIXUP_BINDING', require, 0 <= value <= 65535, 'Own code offset overflow')
            struct.pack_into('<H', payload, at, value)
            tables.append({'offset': at, 'target_offset': target, 'linked_value': value,
                           'beyond_bound_interval': target >= end})
    specs, contribution, proof, data_relocations, targets = {}, {}, [], [], {}
    if own_data:
        from secondary_contribution import bind_secondary
        specs = _step('TU_DATA', ctx.data_specs, obj, own_data)
        targets = _step('TU_DATA', secondary_targets, obj, specs)
        view2 = copy.copy(obj)
        view2.segment_lengths = {n: (size if n == segment or n in specs else 0)
                                 for n, size in obj.segment_lengths.items()}
        view2.linker_fixups = ([f for f in obj.linker_fixups if f['segment'] in specs] + own_data)
        record = {'object_segment': segment, 'start': S, 'secondary_dgroup_segments': specs,
                  'secondary_external_targets': targets}
        linked, contribution, proof, data_relocations = _step(
            'TU_DATA', bind_secondary, view2, record, image, ctx.relocations,
            bytes(payload), own_data)
        payload = bytearray(linked)
    return bytes(payload), {
        'external_mode': mode, 'external': receipt, 'internal_calls': rows,
        'secondary_dgroup_segments': specs, 'secondary_external_targets': targets,
        'secondary_dgroup_fixups': proof,
        'secondary_payloads': {n: b.hex() for n, b in contribution.items()},
        'secondary_generated_relocations': data_relocations,
        'local_code_offsets': tables,
        'generated_relocations': stream}


def _codegen(code, rec, image, S, relocations):
    fields = {i for f in rec['fixups'] for i in range(f['offset'], f['offset'] + f['width'])}
    diff = [i for i in range(rec['start'], rec['end'])
            if i not in fields and (S + i >= len(image) or code[i] != image[S + i])]
    sites = {r['load_offset'] for r in relocation_stream(rec['fixups'], S)}
    oracle = {r['load_offset'] for r in oracle_slice(relocations, S, rec['start'], rec['end'])}
    return {'exact': not diff and sites == oracle, 'byte_differences': len(diff),
            'first_difference': diff[0] if diff else None,
            'relocation_sites_agree': sites == oracle}


def _check_interval(obj, segment, lo, hi, fixes, ctx, length):
    """Bind [0,hi) with ``fixes``; prove [lo,hi) exact, with its ordered MZ entries."""
    S, image = ctx.start, ctx.image
    _step('PUBLIC_ENTRY', _check_publics, obj, segment, ctx, lo, hi, length)
    payload, receipt = bind_range(obj, segment, hi, fixes, ctx)
    got, want = payload[lo:hi], image[S + lo:S + hi]
    if got != want:
        first = next((i for i, (a, b) in enumerate(zip(got, want)) if a != b), min(len(got), len(want)))
        raise PrefixFailure('BYTE_MISMATCH', f'bound bytes differ at object offset {lo + first} '
                            f'(load {S + lo + first})')
    ordered = oracle_slice(ctx.relocations, S, lo, hi)
    _step('RELOCATION_ORDER', require, receipt['generated_relocations'] == ordered,
          'Candidate FIXUPP-derived relocation stream differs from the oracle subsequence')
    return payload, receipt


def _suffix_dependencies(code, end, receipt):
    """Closed-record bytes that depend on code after the prefix (diagnostic listing)."""
    result = {'near_calls': [r for r in receipt['internal_calls'] if r['beyond_bound_interval']],
              'code_offsets': [], 'relative_branches': []}
    result['code_offsets'] = [r for r in receipt['local_code_offsets'] if r['beyond_bound_interval']]
    try:
        from diagnostics import _decode
        tables = {r['offset'] for r in receipt['local_code_offsets']}
        pos = 0
        while pos < end:
            if pos in tables:
                pos += 2
                continue
            rows = _decode(code[pos:pos + 16], pos)
            if not rows:
                result['relative_branches'] = 'UNDECODABLE'
                break
            row = rows[0]
            size = len(bytes.fromhex(row['bytes']))
            if (row['control'] == 'jump' and row['operands'] and
                    row['operands'][0].get('kind') == 'imm' and
                    code[pos] not in (0x9a, 0xea) and row['operands'][0]['value'] >= end):
                result['relative_branches'].append({'offset': pos, 'target_offset': row['operands'][0]['value']})
            pos += size
    except Exception as error:  # diagnostic only; the proof compares bytes
        result['relative_branches'] = 'DECODER_UNAVAILABLE: ' + str(error)
    return result


def derive(obj, ctx, segment='UNIT_TEXT'):
    """Fresh proof for one compiled candidate object. Returns (proof, bound)."""
    records = candidate_records(obj, segment)
    S, image, code = ctx.start, ctx.image, obj.segment_bytes(segment)
    length = records[-1]['end']
    publics = _publics(obj, segment)
    rows = []
    for rec in records:
        row = {'index': rec['index'], 'start': rec['start'], 'end': rec['end'],
               'load_start': S + rec['start'], 'load_end': S + rec['end'],
               'fixup_count': len(rec['fixups']), 'closed': rec['index'] < len(records) - 1,
               'codegen': _codegen(code, rec, image, S, ctx.relocations)}
        try:
            _check_interval(obj, segment, rec['start'], rec['end'], rec['fixups'], ctx, length)
            row['bound_exact'] = True
        except PrefixFailure as error:
            row['bound_exact'] = False
            row['failure'] = {'category': error.category, 'message': error.detail}
        rows.append(row)
    k = 0
    local = {p['name'] for p in obj.local_publics}
    try:
        _step('OBJECT_START', ctx.object_start, publics[0]['name'], publics[0]['name'] in local)
        start_failure = None
    except PrefixFailure as error:
        start_failure = {'record': 0, 'category': error.category, 'message': error.detail}
    while not start_failure and k < len(rows) and rows[k]['closed'] and rows[k]['bound_exact']:
        k += 1
    if start_failure:
        stop = start_failure
    elif k == len(rows):
        stop = None
    elif rows[k]['bound_exact']:
        stop = {'record': k, 'category': 'OPEN_RECORD',
                'message': 'final candidate record is not naturally closed'}
    else:
        stop = {'record': k, **rows[k]['failure']}
    bound, shrunk = None, []
    while k:
        fixes = [f for rec in records[:k] for f in rec['fixups']]
        try:
            payload, receipt = _check_interval(obj, segment, 0, records[k - 1]['end'], fixes, ctx, length)
            bound = {'payload': payload, 'receipt': receipt, 'fixups': fixes}
            break
        except PrefixFailure as error:
            shrunk.append({'records': k, 'category': error.category, 'message': error.detail})
            k -= 1
    end = records[k - 1]['end'] if k else 0
    for row in rows:
        if row['index'] < k:
            row['state'] = 'RECORD_CLOSED_EXACT'
        elif row['bound_exact']:
            row['state'] = 'BOUND_EXACT_AFTER_GAP' if row['closed'] else 'EXACT_OPEN_RECORD'
        else:
            row['state'] = 'CODEGEN_EXACT' if row['codegen']['exact'] else 'MISMATCH'
    offsets = [p['offset'] for p in publics] + [length]
    members = []
    for index, p in enumerate(publics):
        lo, hi = p['offset'], offsets[index + 1]
        covering = [r for r in rows if r['start'] < hi and lo < r['end']]
        if hi <= end:
            state = 'RECORD_CLOSED_EXACT'
        elif all(r['bound_exact'] for r in covering):
            state = 'EXACT_OPEN_RECORD'
        elif all(r['codegen']['exact'] for r in covering):
            state = 'CODEGEN_EXACT'
        else:
            state = 'NOT_EXACT'
        members.append({'public': p['name'], 'start_offset': lo, 'end_offset': hi,
                        'load_start': S + lo, 'load_end': S + hi, 'state': state,
                        'partial_prefix': lo < end < hi})
    cut = next((m['public'] for m in members if m['partial_prefix']), None)
    proof = {'schema': SCHEMA, 'object_start': S, 'object_segment': segment,
             'candidate_object_length': length, 'record_count': len(records),
             'records': rows, 'members': members,
             'prefix': {'records': k, 'end_offset': end, 'start': S, 'end': S + end,
                        'record_ends': [r['end'] for r in records[:k]],
                        'ends_mid_function': cut, 'stopped_by': stop,
                        'cumulative_shrinks': shrunk,
                        'external_mode': bound['receipt']['external_mode'] if bound else None,
                        'suffix_dependencies': (_suffix_dependencies(bound['payload'], end,
                                                                     bound['receipt'])
                                                if bound else None)},
             'complete_object_candidate': all(r['bound_exact'] for r in rows)}
    return proof, bound


def classifier_view(proof, accepted=()):
    """Stable API for the diagnostic classifier: closed intervals and member states."""
    owned = [(a, b) for a, b in accepted]
    members = {}
    for m in proof['members']:
        state = m['state']
        if any(a <= m['load_start'] and m['load_end'] <= b for a, b in owned):
            state = 'ACCEPTED'
        members[m['public'].lstrip('_')] = state
    p = proof['prefix']
    return {'schema': SCHEMA + '/classifier-v1', 'members': members,
            'closed_intervals': ([{'start': p['start'], 'end': p['end'],
                                   'state': 'RECORD_CLOSED_EXACT'}] if p['records'] else []),
            'open_intervals': [{'start': r['load_start'], 'end': r['load_end'], 'state': r['state']}
                               for r in proof['records'] if r['state'] != 'RECORD_CLOSED_EXACT']}


def prefix_members(proof, ctx, obj):
    """Inventory rows of the members in the prefix; complete ones carry their hash."""
    local = {p['name'] for p in obj.local_publics}
    out = []
    for m in proof['members']:
        if m['start_offset'] >= proof['prefix']['end_offset']:
            continue
        f = ctx.entry(m['public'], m['load_start'], m['public'] in local)
        row = {'name': f['name'], 'public': m['public'], 'stable_id': f.get('stable_id'),
               'start': f['start'], 'end': f['end'],
               'complete': f['end'] <= proof['prefix']['end']}
        if row['complete']:
            row['target'] = {'size': f['size'], 'sha256': f['sha256']}
        if m['public'] in local:
            row['local_symbol'] = True
        out.append(row)
    return out


def _declarations(obj):
    return {'segments': obj.segment_defs, 'groups': obj.groups,
            'publics': obj.publics, 'externals': obj.externals}


def draft_recipe(proof, bound, obj, ctx, *, name, source, profile, flags=None, register=None,
                 records=None):
    """Recipe for the owned prefix (records 0..records-1, at most the derived prefix)."""
    p = proof['prefix']
    k = p['records'] if records is None else records
    require(0 < k <= p['records'], 'Owned prefix must be a nonempty part of the derived prefix')
    end = p['record_ends'][k - 1]
    S = proof['object_start']
    fixes = [f for f in bound['fixups'] if f['offset'] < end]
    payload, receipt = bind_range(obj, proof['object_segment'], end, fixes, ctx)
    cut = next((m['public'] for m in proof['members'] if m['start_offset'] < end < m['end_offset']), None)
    recipe = {'id': name, 'stable_id': name, 'kind': 'c', 'profile': profile, 'source': source,
              'preprocessor_closure': [], 'object_segment': proof['object_segment'],
              'start': S, 'end': S + end, 'target': identity(ctx.image[S:S + end]),
              'prefix_of_object': {'schema': SCHEMA, 'object_start': S, 'records': k,
                                   'record_ends': p['record_ends'][:k],
                                   'candidate_object_length': proof['candidate_object_length'],
                                   'ends_mid_function': cut},
              'prefix_members': [m for m in prefix_members(proof, ctx, obj) if m['start'] < S + end],
              'object_declarations': _declarations(obj), 'expected_fixups': fixes,
              'expected_relocations': oracle_slice(ctx.relocations, S, 0, end)}
    for m in recipe['prefix_members']:
        m['complete'] = m['end'] <= S + end
        if not m['complete']:
            m.pop('target', None)
    if receipt['external_mode']:
        recipe['external_binding'] = {'mode': receipt['external_mode']}
    if receipt['secondary_dgroup_segments']:
        recipe['secondary_dgroup_segments'] = receipt['secondary_dgroup_segments']
        recipe['secondary_external_targets'] = receipt['secondary_external_targets']
    if receipt['local_code_offsets']:
        recipe['original_frame_load_address'] = ctx.frame()
    if flags is not None:
        recipe['compiler_flags'], recipe['compiler_flags_register'] = list(flags), register
    return recipe


def check_recipe_form(recipe):
    """Prefix recipes carry only proof inputs; diagnostic results cannot enter."""
    keys = set(recipe)
    require(REQUIRED_KEYS <= keys and keys <= REQUIRED_KEYS | OPTIONAL_KEYS,
            'Prefix recipe fields differ from the record-closed prefix form: ' +
            ', '.join(sorted(keys - REQUIRED_KEYS - OPTIONAL_KEYS) or
                      sorted(REQUIRED_KEYS - keys)))
    meta = recipe['prefix_of_object']
    require(isinstance(meta, dict) and set(meta) == META_KEYS and meta['schema'] == SCHEMA and
            meta['object_start'] == recipe['start'] and recipe.get('kind') == 'c' and
            type(meta['records']) is int and meta['records'] > 0 and
            meta['record_ends'] and len(meta['record_ends']) == meta['records'] and
            meta['record_ends'][-1] == recipe['end'] - recipe['start'],
            'Prefix must start at the candidate object start and end at a closed record')


def bind_prefix(obj, recipe, image, relocations, ctx=None):
    """Production binder: re-derive the proof from scratch, never trust a stored one."""
    require('relocation_order_basis' not in recipe, 'Recipe cannot supply a relocation order basis')
    check_recipe_form(recipe)
    segment = recipe['object_segment']
    ctx = ctx or OracleContext(image, relocations, recipe['start'])
    require(ctx.start == recipe['start'], 'Prefix context start differs')
    require(recipe['object_declarations'] == _declarations(obj),
            'Complete candidate object declarations differ')
    proof, bound = derive(obj, ctx, segment)
    meta, owned = recipe['prefix_of_object'], recipe['end'] - recipe['start']
    derived = proof['prefix']
    require(derived['end_offset'] >= owned,
            f'Re-derived RECORD_CLOSED_EXACT prefix ({derived["end_offset"]} bytes, '
            f'{derived["records"]} records) is shorter than the owned interval ({owned} bytes)')
    require(derived['record_ends'][:meta['records']] == meta['record_ends'] and
            proof['candidate_object_length'] == meta['candidate_object_length'],
            'Owned prefix does not end at the re-derived candidate record boundary')
    fixes = [f for f in bound['fixups'] if f['offset'] < owned]
    require(fixes == recipe['expected_fixups'], 'Owned prefix ordered FIXUPPs differ')
    payload, receipt = _check_interval(obj, segment, 0, owned, fixes, ctx,
                                       proof['candidate_object_length'])
    expected = draft_recipe(proof, bound, obj, ctx, name=recipe['id'], source=recipe['source'],
                            profile=recipe['profile'], flags=recipe.get('compiler_flags'),
                            register=recipe.get('compiler_flags_register'),
                            records=meta['records'])
    for key in ('prefix_of_object', 'prefix_members', 'external_binding',
                'secondary_dgroup_segments', 'secondary_external_targets',
                'original_frame_load_address', 'target', 'expected_relocations'):
        require(recipe.get(key) == expected.get(key), 'Prefix recipe differs from re-derived ' + key)
    require(receipt['generated_relocations'] == recipe['expected_relocations'],
            'Prefix relocation obligations differ')
    summary = {'owner': recipe['id'], 'owned_records': meta['records'], 'owned_end_offset': owned,
               'derived_records': derived['records'], 'derived_end_offset': derived['end_offset'],
               'candidate_record_count': proof['record_count'],
               'record_states': [r['state'] for r in proof['records']],
               'ends_mid_function': meta['ends_mid_function'],
               'suffix_dependencies': derived['suffix_dependencies']}
    return payload, {'mode': SCHEMA, 'prefix': summary, 'binding': receipt['external'],
                     'internal_calls': receipt['internal_calls'],
                     'secondary_dgroup_fixups': receipt['secondary_dgroup_fixups'],
                     'secondary_payloads': receipt['secondary_payloads'],
                     'secondary_generated_relocations': receipt['secondary_generated_relocations'],
                     'generated_relocations': receipt['generated_relocations']}


def compile_candidate(source, profile='msc510-medium', segment='UNIT_TEXT', flags='auto',
                     communal_declarations=None):
    """Compile the whole candidate TU; registered object flags follow its start entry."""
    from compiler import compile_source, CompileFailure
    from object_flags import registered_objects
    from object_probe import msc_alignment_sparse_zero
    communal_recipe = None
    communal_names = None
    if communal_declarations is not None:
        from communal_unit import recipe_declarations, check_object_communals
        communal_recipe = {'communal_declarations': communal_declarations}
        declared = recipe_declarations(communal_recipe)
        communal_names = [name for name, _ in declared]

    def compile_source(source, profile, flags=None, _compile=compile_source):
        # MSC word-alignment holes in the TU's _DATA/CONST are read with the
        # same reviewed policy shape a recipe carries (object_probe).
        try:
            result = _compile(source, profile, flags, communals=communal_names)
        except CompileFailure as error:
            path = Path(error.receipt.get('work_directory') or '.') / 'UNIT.OBJ'
            if 'Holes' not in str(error) or not path.exists():
                raise
            policy = msc_alignment_sparse_zero(path.read_bytes())
            if policy is None:
                raise
            result = _compile(source, profile, flags, sparse_zero=policy,
                              communals=communal_names)
        if communal_recipe is not None:
            check_object_communals(result[0], communal_recipe)
        return result
    obj, receipt = compile_source(source, profile)
    chosen = register = None
    if flags == 'auto':
        from oracle import verify
        from mz import MZ
        oracle = verify(write=False)
        image = MZ.parse(oracle[1]).load_image(oracle[1])
        ctx = OracleContext(image, oracle[2]['unpacked_mz']['relocations'], 0)
        first = _publics(obj, segment)[0]['name']
        rows = [f for f in ctx.rows if '_' + f['name'] == first or f['name'] == first]
        if len(rows) == 1 and rows[0].get('segment') in registered_objects():
            register, chosen = registered_objects()[rows[0]['segment']]
            obj, receipt = compile_source(source, profile, chosen)
    return obj, receipt, chosen, register


def prove_source(source, *, profile='msc510-medium', segment='UNIT_TEXT', flags='auto',
                 recipe_id=None, recipe_source=None, records=None,
                 communal_declarations=None):
    """API for research tools (e.g. the diagnostic classifier): compile a whole
    candidate TU, derive its proof against the locked oracle, and return a
    JSON-serializable report. It never grants ownership."""
    from oracle import verify
    from mz import MZ
    obj, receipt, chosen, register = compile_candidate(
        source, profile, segment, flags, communal_declarations)
    oracle = verify(write=False)
    image = MZ.parse(oracle[1]).load_image(oracle[1])
    relocations = oracle[2]['unpacked_mz']['relocations']
    first = _publics(obj, segment)[0]['name']
    base = OracleContext(image, relocations, 0)
    rows = [f for f in base.rows if '_' + f['name'] == first or f['name'] == first]
    require(len(rows) == 1, f'Object start public {first} has no unique inventory entry')
    ctx = OracleContext(image, relocations, rows[0]['start'])
    proof, bound = derive(obj, ctx, segment)
    manifest = read_json(ROOT/'layout/manifest.json')
    accepted = [(o['start'], o['end']) for o in manifest['owners']
                if o['kind'] in ('MATCHING_C', 'MATCHING_ASM')]
    report = {'source': identity(source), 'compiler_flags': chosen,
              'compiler_flags_register': register, 'object': receipt['object'],
              'proof': proof, 'classifier': classifier_view(proof, accepted),
              'authority': 'Research report; ownership requires promote.py and fresh validation.'}
    if recipe_id and proof['prefix']['records']:
        from preprocessor import prepare
        recipe = draft_recipe(proof, bound, obj, ctx, name=recipe_id, source=recipe_source,
                              profile=profile, flags=chosen, register=register, records=records)
        recipe['preprocessor_closure'] = prepare(source, profile)[1]
        if receipt.get('sparse_zero'):
            recipe['sparse_zero'] = receipt['sparse_zero']
        report['recipe'] = recipe
    return report


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__.split('Example')[0])
    p.add_argument('source')
    p.add_argument('--segment', default='UNIT_TEXT')
    p.add_argument('--profile', default='msc510-medium')
    p.add_argument('--canonical-flags', action='store_true',
                   help='Do not apply the registered per-object flag set')
    p.add_argument('--report', help='Machine-readable proof report (JSON)')
    p.add_argument('--recipe-out', help='Draft prefix recipe for promote.py --verify-only')
    p.add_argument('--id', help='Owner id for the draft recipe')
    p.add_argument('--records', type=int, help='Own fewer records than derived')
    a = p.parse_args(argv)
    from common import write_json
    from pathlib import Path
    require(not a.recipe_out or a.id, '--recipe-out needs --id')
    try:
        report = prove_source(Path(a.source).read_bytes(), profile=a.profile, segment=a.segment,
                              flags='none' if a.canonical_flags else 'auto', recipe_id=a.id,
                              recipe_source=Path(a.source).as_posix(), records=a.records)
    except ValueError as error:
        print(f'{a.source}: NO_PROOF: {error}')
        return None
    report['path'] = a.source
    if a.recipe_out and 'recipe' in report:
        write_json(Path(a.recipe_out), report['recipe'])
    if a.report:
        write_json(Path(a.report), report)
    proof = report['proof']
    pre = proof['prefix']
    print(f"{a.source}: object start {pre['start']}, {proof['record_count']} candidate CODE records, "
          f"RECORD_CLOSED_EXACT prefix {pre['records']} records / {pre['end_offset']} bytes "
          f"[{pre['start']},{pre['end']})" +
          (f"; stopped at record {pre['stopped_by']['record']}: {pre['stopped_by']['category']} "
           f"{pre['stopped_by']['message'][:160]}" if pre['stopped_by'] else ''))
    for row in proof['records']:
        print(f"  record {row['index']:2d} [{row['start']:5d},{row['end']:5d}) {row['state']:22s}"
              + (f" {row['failure']['category']}: {row['failure']['message'][:110]}"
                 if 'failure' in row else ''))
    return report


if __name__ == '__main__':
    main()
