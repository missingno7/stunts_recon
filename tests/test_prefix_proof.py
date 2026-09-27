"""RECORD_CLOSED_EXACT prefix proof: candidate-derived records and relocation order.

Synthetic translation units are compiled with the pinned MSC 5.10 compiler; a
small independent linker model in this file places the reference object into a
synthetic oracle (far CALLs to fixed symbols, same-object near CALLs, MZ
entries in record/FIXUPP order). The real-oracle cases use canonical sources.
"""
import copy
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from common import identity, read_json, require, sha
from compiler import compile_source
import prefix_proof
from prefix_proof import (bind_prefix, candidate_records, check_recipe_form, derive,
                          draft_recipe, relocation_stream)

SEGMENT = 'UNIT_TEXT'
START = 0x10000
SYMBOLS = {'_ext_a': {'kind': 'far-code', 'frame_load_address': 0x30000, 'load_address': 0x30010},
           '_ext_b': {'kind': 'far-code', 'frame_load_address': 0x30000, 'load_address': 0x30230}}


def _body(tag, count):
    return ''.join(f'    r = r + ext_a(a + {tag * 100 + j}) * {j + 3};\n'
                   f'    b = b ^ ext_b(r, {j + tag});\n' for j in range(count))


def unit(*, sizes=(10,) * 5, f6=24, f7=True, call7=False, extra0='', tail6='', first6=None,
         body7='    return a + 1;\n'):
    """f0 calls f5 (and f7 with call7); f6 spans the record the short unit leaves open."""
    out = ['extern int far ext_a(int);', 'extern int far ext_b(int, int);',
           'int f5(int);', 'int f7(int);']
    for i, count in enumerate(sizes):
        calls = ('    r = r + f5(b);\n' if i == 0 else '') + ('    r = r + f7(a);\n' if i == 0 and call7 else '')
        out.append(f'int f{i}(int a, int b)\n{{\n    int r;\n    r = 0;\n' + (extra0 if i == 0 else '') +
                   calls + _body(i, count) + '    return r + b;\n}\n')
    out.append('int f5(int a)\n{\n    return ext_a(a) + 2;\n}\n')
    if f6:
        out.append('int f6(int a, int b)\n{\n    int r;\n    r = 0;\n' + (first6 or '') +
                   _body(6, f6) + tail6 + '    return r;\n}\n')
    if f7:
        out.append('int f7(int a)\n{\n' + body7 + '}\n')
    return '\n'.join(out).encode('ascii')


_CACHE = {}


def compiled(**kwargs):
    key = repr(sorted(kwargs.items()))
    if key not in _CACHE:
        _CACHE[key] = compile_source(unit(**kwargs), 'msc510-medium')[0]
    return _CACHE[key]


def link(obj):
    """Independent reference linker: returns (image, ordered MZ entries, entries)."""
    code = bytearray(obj.segment_bytes(SEGMENT))
    publics = {p['name']: p['offset'] for p in obj.publics if p['segment'] == SEGMENT}
    relocations = []
    for fix in obj.linker_fixups:
        at = fix['offset']
        if fix['loc'] == 'pointer32':
            symbol = SYMBOLS[fix['target']]
            struct.pack_into('<HH', code, at, symbol['load_address'] - symbol['frame_load_address'],
                             symbol['frame_load_address'] // 16)
            site = START + at + 2
            relocations.append({'segment': (site // 65536) * 4096, 'offset': site % 65536,
                                'load_offset': site})
        else:
            assert fix['self_relative'] and fix['target'] in publics, fix
            struct.pack_into('<h', code, at, publics[fix['target']] - (at + 2))
    image = bytes(b'\xcc' * START) + bytes(code) + b'\xcc' * 64
    ordered = sorted(publics.items(), key=lambda item: item[1]) + [(None, len(code))]
    entries = [{'name': name[1:], 'public': name, 'start': START + offset,
                'end': START + ordered[i + 1][1], 'stable_id': 'syn_' + name[1:]}
               for i, (name, offset) in enumerate(ordered[:-1])]
    for row in entries:
        row.update(size=row['end'] - row['start'], sha256=sha(image[row['start']:row['end']]))
    return image, relocations, entries


class SyntheticContext:
    """Stands in for the oracle registries: same interface as OracleContext."""

    def __init__(self, image, relocations, entries, start=START, object_address=START):
        self.image, self.relocations, self.start = image, relocations, start
        self.rows, self.object_address = entries, object_address

    def entry(self, public, address, local):
        rows = [f for f in self.rows if f['start'] == address]
        require(len(rows) == 1 and rows[0]['public'] == public and not local,
                f'Candidate public {public} at load {address} is not one inventory entry')
        return rows[0]

    def object_start(self, public, local):
        require(self.start == self.object_address, 'Object start is not the start of its original code frame')
        return self.entry(public, self.start, local)

    def rows_in(self, lo, hi):
        return [f for f in self.rows if f['start'] < hi and lo < f['end']]

    def frame(self):
        return self.object_address

    def resolve(self, sub):
        return {name: SYMBOLS[name] for name in {f['target'] for f in sub['expected_fixups']}}

    def data_specs(self, obj, own):
        raise ValueError('no TU-owned data in synthetic units')


def oracle_of(**kwargs):
    return link(compiled(**kwargs))


def prove(candidate, oracle, **context):
    image, relocations, entries = oracle
    return derive(candidate, SyntheticContext(image, relocations, entries, **context))


class CandidateRecords(unittest.TestCase):
    def test_records_come_from_the_candidate_object(self):
        obj = compiled()
        records = candidate_records(obj, SEGMENT)
        self.assertGreaterEqual(len(records), 4)
        self.assertEqual([f for r in records for f in r['fixups']], obj.linker_fixups)
        # FIXUPPs descend within each record; the stream keeps that order.
        for record in records:
            offsets = [f['offset'] for f in record['fixups']]
            self.assertEqual(offsets, sorted(offsets, reverse=True))
        stream = relocation_stream(obj.linker_fixups, START)
        self.assertNotEqual([r['load_offset'] for r in stream],
                            sorted(r['load_offset'] for r in stream))

    def test_missing_omf_fails_closed(self):
        obj = copy.copy(compiled())
        obj.omf_bytes = None
        with self.assertRaisesRegex(ValueError, 'emitted OMF'):
            candidate_records(obj, SEGMENT)


class PrefixDerivation(unittest.TestCase):
    def test_reference_prefix_is_every_closed_record(self):
        proof, bound = prove(compiled(), oracle_of())
        records = proof['records']
        self.assertEqual(proof['prefix']['records'], len(records) - 1)
        self.assertEqual([r['state'] for r in records[:-1]], ['RECORD_CLOSED_EXACT'] * (len(records) - 1))
        self.assertEqual(records[-1]['state'], 'EXACT_OPEN_RECORD')
        self.assertEqual(proof['prefix']['stopped_by']['category'], 'OPEN_RECORD')
        self.assertTrue(proof['complete_object_candidate'])
        end = proof['prefix']['end_offset']
        self.assertEqual(bound['payload'], oracle_of()[0][START:START + end])
        self.assertEqual(proof['prefix']['ends_mid_function'], '_f6')

    def test_reversed_oracle_relocation_order_fails(self):
        # (1) Identical bytes; the oracle order inside record 1 is reversed.
        image, relocations, entries = oracle_of()
        records = candidate_records(compiled(), SEGMENT)
        lo, hi = START + records[1]['start'], START + records[1]['end']
        inside = [r for r in relocations if lo <= r['load_offset'] < hi]
        altered = [r for r in relocations if r['load_offset'] < lo] + inside[::-1] + \
                  [r for r in relocations if r['load_offset'] >= hi]
        proof, _ = prove(compiled(), (image, altered, entries))
        self.assertEqual(proof['prefix']['records'], 1)
        row = proof['records'][1]
        self.assertTrue(row['codegen']['exact'])
        self.assertEqual(row['failure']['category'], 'RELOCATION_ORDER')

    def test_cross_record_order_is_checked_cumulatively(self):
        # Each record's own subsequence is unchanged; their interleaving is not.
        image, relocations, entries = oracle_of()
        records = candidate_records(compiled(), SEGMENT)
        boundary = START + records[1]['end']
        moved = next(r for r in relocations if r['load_offset'] >= boundary)
        altered = [r for r in relocations if r is not moved]
        altered.insert(0, moved)
        proof, _ = prove(compiled(), (image, altered, entries))
        self.assertTrue(all(r['bound_exact'] for r in proof['records']))
        self.assertLess(proof['prefix']['records'], len(records) - 1)
        self.assertTrue(proof['prefix']['cumulative_shrinks'])
        self.assertEqual(proof['prefix']['cumulative_shrinks'][0]['category'], 'RELOCATION_ORDER')

    def test_suffix_change_without_dependency_keeps_prefix(self):
        # (3a) f7 lies wholly in the open record and no closed byte depends on it.
        reference, _ = prove(compiled(call7=True), oracle_of(call7=True))
        changed, _ = prove(compiled(call7=True, body7='    return a * 7 + ext_a(a);\n'),
                           oracle_of(call7=True))
        for key in ('records', 'end_offset', 'record_ends', 'ends_mid_function',
                    'external_mode', 'suffix_dependencies'):
            self.assertEqual(changed['prefix'][key], reference['prefix'][key])
        self.assertEqual([r['state'] for r in changed['records'][:-1]],
                         [r['state'] for r in reference['records'][:-1]])
        self.assertNotEqual(changed['records'][-1]['state'], 'EXACT_OPEN_RECORD')
        near = reference['prefix']['suffix_dependencies']['near_calls']
        self.assertEqual([r['target'] for r in near], ['_f7'])

    def test_suffix_change_with_forward_dependency_invalidates_prefix(self):
        # (3b) Growing f6's tail moves f7; record 0's near CALL displacement changes.
        proof, _ = prove(compiled(call7=True, tail6='    r = r + ext_a(r);\n'),
                         oracle_of(call7=True))
        self.assertEqual(proof['prefix']['records'], 0)
        self.assertIn(proof['records'][0]['failure']['category'], ('PUBLIC_ENTRY', 'BYTE_MISMATCH'))

    def test_open_record_is_never_counted_and_closing_it_moves_prefix(self):
        # (4) Without f6/f7 the record holding f5 is the open final record.
        oracle = oracle_of()
        short, _ = prove(compiled(f6=0, f7=False), oracle)
        full, _ = prove(compiled(), oracle)
        self.assertEqual(short['records'][-1]['state'], 'EXACT_OPEN_RECORD')
        self.assertEqual(short['prefix']['records'] + 1, full['prefix']['records'])
        # A changed FIXUPP inside that record when it closes stops the prefix.
        changed, _ = prove(compiled(first6='    r = ext_b(a, 7);\n'), oracle)
        self.assertEqual(changed['prefix']['records'], short['prefix']['records'])
        self.assertNotEqual(changed['records'][short['prefix']['records']]['state'],
                            'RECORD_CLOSED_EXACT')

    def test_added_fixup_changes_candidate_stream_and_cut(self):
        # (5) One more far CALL before record 0 closes.
        base = compiled()
        added = compiled(extra0='    r = ext_b(a, 99);\n')
        old, new = candidate_records(base, SEGMENT), candidate_records(added, SEGMENT)
        self.assertEqual(len(new[0]['fixups']), len(old[0]['fixups']) + 1)
        self.assertNotEqual(relocation_stream(new[0]['fixups'], START),
                            relocation_stream(old[0]['fixups'], START))
        self.assertNotEqual([r['end'] for r in new], [r['end'] for r in old])
        proof, _ = prove(added, oracle_of())
        self.assertEqual(proof['prefix']['records'], 0)

    def test_prefix_starts_only_at_the_candidate_object_start(self):
        image, relocations, entries = oracle_of()
        f1 = next(e for e in entries if e['public'] == '_f1')
        proof, _ = prove(compiled(), (image, relocations, entries), start=f1['start'])
        self.assertEqual(proof['prefix']['records'], 0)
        self.assertEqual(proof['prefix']['stopped_by']['category'], 'OBJECT_START')
        proof, _ = prove(compiled(sizes=(10,) * 4), oracle_of(), start=f1['start'])
        self.assertEqual(proof['prefix']['stopped_by']['category'], 'OBJECT_START')


class PrefixOwnership(unittest.TestCase):
    def setUp(self):
        self.obj = compiled()
        self.image, self.relocations, entries = oracle_of()
        self.ctx = SyntheticContext(self.image, self.relocations, entries)
        proof, bound = derive(self.obj, self.ctx)
        self.recipe = draft_recipe(proof, bound, self.obj, self.ctx, name='prefix_syn',
                                   source='src/prefix_syn.c', profile='msc510-medium')

    def bind(self, recipe, obj=None):
        return bind_prefix(obj or self.obj, recipe, self.image, self.relocations, ctx=self.ctx)

    def test_owned_prefix_rebinds_from_scratch(self):
        payload, receipt = self.bind(self.recipe)
        end = self.recipe['end'] - self.recipe['start']
        self.assertEqual(payload, self.image[START:START + end])
        self.assertEqual(receipt['generated_relocations'], self.recipe['expected_relocations'])
        self.assertEqual(receipt['prefix']['record_states'][-1], 'EXACT_OPEN_RECORD')

    def test_shorter_rederived_prefix_fails_instead_of_shrinking(self):
        # A changed oracle byte in record 1 (outside every FIXUPP field).
        records = candidate_records(self.obj, SEGMENT)
        fields = {START + i for f in self.obj.linker_fixups
                  for i in range(f['offset'], f['offset'] + f['width'])}
        at = next(i for i in range(START + records[1]['start'], START + records[1]['end'])
                  if i not in fields)
        image = bytearray(self.image)
        image[at] ^= 0xFF
        ctx = SyntheticContext(bytes(image), self.relocations, self.ctx.rows)
        with self.assertRaisesRegex(ValueError, 'shorter than the owned interval'):
            bind_prefix(self.obj, self.recipe, bytes(image), self.relocations, ctx=ctx)
        claimed = copy.deepcopy(self.recipe)
        meta = claimed['prefix_of_object']
        ends = [r['end'] for r in candidate_records(self.obj, SEGMENT)]
        meta['records'] += 1
        meta['record_ends'] = ends[:meta['records']]
        claimed['end'] = START + ends[meta['records'] - 1]
        with self.assertRaisesRegex(ValueError, 'shorter than the owned interval'):
            self.bind(claimed)

    def test_relocation_order_basis_remains_rejected(self):
        # (2) Neither the prefix binder nor the probe entry accepts it.
        from probe_module import probe
        wrong = {**self.recipe, 'relocation_order_basis': 'oracle-complete-interval-v1'}
        with self.assertRaisesRegex(ValueError, 'relocation order basis'):
            self.bind(wrong)
        with self.assertRaisesRegex(ValueError, 'relocation order basis'):
            probe(wrong)

    def test_diagnostic_results_cannot_grant_ownership(self):
        # (6) Simulator, classifier, blockdiff and tubench outputs are not inputs.
        for key in ('observed_record_ends', 'cut_simulator', 'record_proof', 'classifier',
                    'blockdiff', 'tubench', 'members', 'record_closed_exact'):
            with self.assertRaisesRegex(ValueError, 'Prefix recipe fields differ'):
                check_recipe_form({**self.recipe, key: True})
        tampered = copy.deepcopy(self.recipe)
        tampered['expected_relocations'] = tampered['expected_relocations'][::-1]
        with self.assertRaises(ValueError):
            self.bind(tampered)
        from build_exact import validate_layout
        for owner in ({'kind': 'RECORD_CLOSED_EXACT'}, {'kind': 'UNRESOLVED_RAW',
                                                        'contribution_form': 'prefix_of_object'}):
            manifest = {'owners': [{'id': 'x', 'start': 0, 'end': 4, **owner}]}
            with self.assertRaises(ValueError):
                validate_layout(manifest, 4)

    def test_recipe_start_must_be_object_start(self):
        wrong = copy.deepcopy(self.recipe)
        wrong['prefix_of_object']['object_start'] += 2
        with self.assertRaisesRegex(ValueError, 'candidate object start'):
            check_recipe_form(wrong)


class RealOraclePrefix(unittest.TestCase):
    """Canonical sources against the locked image and registries."""

    @classmethod
    def setUpClass(cls):
        from oracle import verify
        from mz import MZ
        oracle = verify(write=False)
        cls.image = MZ.parse(oracle[1]).load_image(oracle[1])
        cls.relocations = oracle[2]['unpacked_mz']['relocations']

    def derive_source(self, source, start):
        obj = compile_source(source, 'msc510-medium')[0]
        return derive(obj, prefix_proof.OracleContext(self.image, self.relocations, start))

    def test_seg031_object_is_one_closed_record_plus_open_record(self):
        # The accepted seg031 owners' sources, in order, compiled as one unit.
        # (Or the one whole-object/prefix source once seg031 is owned that way.)
        owners = [o for o in read_json(ROOT / 'layout/manifest.json')['owners']
                  if 171540 <= o['start'] < 173238 and o['kind'] == 'MATCHING_C']
        recipes = [read_json(ROOT / o['recipe']) for o in owners]
        whole = [r for r in recipes if 'prefix_of_object' in r or
                 (r['start'], r['end']) == (171540, 173238)]
        if whole:
            sources = [whole[0]['source']]
        else:
            self.assertEqual((owners[0]['start'], owners[-1]['end']), (171540, 173238))
            sources = [r['source'] for r in recipes]
        proof, bound = self.derive_source(b''.join((ROOT / s).read_bytes() for s in sources), 171540)
        self.assertEqual([r['state'] for r in proof['records']],
                         ['RECORD_CLOSED_EXACT', 'EXACT_OPEN_RECORD'])
        self.assertEqual(proof['prefix']['end_offset'], 944)
        self.assertGreater(len(bound['receipt']['generated_relocations']), 0)

    def test_start_inside_a_code_frame_is_not_an_object_start(self):
        source = (ROOT / read_json(ROOT / 'recipes/rectlist_core_group.json')['source']).read_bytes()
        proof, _ = self.derive_source(source, 91506)
        self.assertEqual(proof['prefix']['records'], 0)
        self.assertEqual(proof['prefix']['stopped_by']['category'], 'OBJECT_START')


if __name__ == '__main__':
    unittest.main()
