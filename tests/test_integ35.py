"""integ35: raw BSS debt as per-object placeholders in link order, statics placed
after raw predecessors, and the two diagnostic hash models (LINK communals, MSC
file statics) with their fixture corpora."""
import copy
import json
import re
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json  # noqa: E402
import bss_link  # noqa: E402

FIXTURES = ROOT / 'tests' / 'fixtures'


def _partition(manifest, evidence=None):
    layout = read_json(ROOT / 'layout/data-symbols.json')
    return bss_link.check_partition(manifest, read_json(ROOT / 'layout/link-objects.json')['objects'],
                                    evidence or read_json(ROOT / bss_link.PARTITION_EVIDENCE),
                                    layout['bss_start'], layout['bss_end'])


def _unaccepted(manifest, evidence, objects):
    """The manifest with these objects' accepted game `_BSS` rows turned back into
    their reviewed raw placeholders (the refusal fixtures predate integ37)."""
    rows = []
    for row in manifest['bss_owners']:
        obj = row.get('parent')
        if row['kind'] != 'UNRESOLVED_RAW' and obj in objects:
            raw = next(r for r in evidence['rows'] if r.get('object') == obj)
            row = {k: raw[k] for k in ('id', 'object', 'raw_form', 'start', 'end')}
            row['kind'] = 'UNRESOLVED_RAW'
        rows.append(row)
    manifest['bss_owners'] = rows
    return manifest


class PartitionTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.manifest = read_json(ROOT / 'layout/manifest.json')
        cls.evidence = read_json(ROOT / bss_link.PARTITION_EVIDENCE)

    def test_canonical_partition_is_per_object_in_link_order(self):
        rows = _partition(self.manifest)
        objects = [r['object'] for r in rows if r['object']]
        self.assertEqual(objects, ['obj_seg000', 'obj_seg001', 'obj_seg005', 'obj_seg006', 'obj_seg007',
                                   'obj_seg008', 'obj_seg009', 'rt_output.c_120254', 'obj_seg027',
                                   'obj_seg028', 'obj_seg032'])
        accepted = {r['object']: (r['start'], r['end']) for r in rows
                    if r['object'] is not None and r['kind'] not in ('UNRESOLVED_RAW', 'LINK_FILL')}
        # integ36: output.c's 38 B are owned pinned runtime _BSS (link-runtime-member-v1).
        # integ37: seg005's and seg006's complete static sets are accepted game _BSS.
        self.assertEqual(accepted, {'obj_seg000': (199994, 200006), 'obj_seg001': (200006, 200042),
                                    'obj_seg005': (200042, 200332), 'obj_seg006': (200332, 203476),
                                    'obj_seg008': (205384, 205400), 'obj_seg009': (205400, 207000),
                                    'rt_output.c_120254': (207000, 207038), 'obj_seg027': (207038, 207086),
                                    'obj_seg028': (207086, 207382), 'obj_seg032': (207382, 207396)})
        fill = [r for r in rows if r['form'] == bss_link.WORD_FILL]
        self.assertEqual([(r['start'], r['end']) for r in fill], [(205383, 205384)])
        # integ39: [207396,207408) is LINK's c_common paragraph fill (owned LINK_FILL).
        # integ40: the whole communal unit is accepted (LINK_COMMUNAL).
        self.assertEqual((rows[-1]['kind'], rows[-1]['start'], rows[-1]['end']),
                         (bss_link.COMMUNAL_KIND, 207408, 222352))
        self.assertEqual((rows[-2]['kind'], rows[-2]['form'], rows[-2]['start'], rows[-2]['end']),
                         ('LINK_FILL', bss_link.COMMUNAL_FILL_BASIS, 207396, 207408))
        report = bss_link.ownership_report(rows)
        self.assertEqual(report['bytes'], {'accepted': 5456, 'accepted_runtime': 38, 'raw_objects': 1907,
                                           'raw_runtime': 0, 'raw_fill': 1, 'raw_communal': 0,
                                           'link_fill': 12, 'accepted_communal': 14944})
        self.assertEqual(report['accepted_runtime'], [['rt_output.c_120254', 'library_output_120254:_BSS', 38]])

    def test_pre_xob_chain_accounts_for_the_whole_static_region(self):
        rows = [r for r in self.evidence['rows'] if r['raw_form'] != bss_link.COMMUNAL_UNIT]
        self.assertEqual(sum(r['end'] - r['start'] for r in rows) + 12, 207396 - 199994)
        output = next(r for r in rows if r['object'] == 'rt_output.c_120254')
        self.assertEqual((output['start'], output['end'], output['pinned_member']['segment_length']),
                         (207000, 207038, 38))

    def refused(self, mutate, evidence=None, pattern=''):
        ev = copy.deepcopy(evidence or self.evidence)
        manifest = _unaccepted(copy.deepcopy(self.manifest), ev, ('obj_seg005', 'obj_seg006'))
        mutate(manifest, ev)
        with self.assertRaisesRegex(ValueError, pattern):
            _partition(manifest, ev)

    def test_refusals(self):
        def raw(manifest, rid):
            return next(r for r in manifest['bss_owners'] if r['id'] == rid)

        def moved(manifest, ev):
            raw(manifest, 'raw_bss_obj_seg005')['end'] -= 2
            raw(manifest, 'raw_bss_obj_seg006')['start'] -= 2
        self.refused(moved, pattern='reviewed placeholder')

        def unnamed(manifest, ev):
            raw(manifest, 'raw_bss_obj_seg005').pop('object')
        self.refused(unnamed, pattern='reviewed placeholder')

        def even_fill(manifest, ev):
            for rows in (manifest['bss_owners'], ev['rows']):
                a = next(r for r in rows if r['id'] == 'raw_bss_obj_seg007')
                b = next(r for r in rows if r['id'] == 'raw_bss_fill_205383')
                a['end'] -= 1
                b['start'] -= 1
        self.refused(even_fill, pattern='odd byte')

        def communal_early(manifest, ev):
            for rows in (manifest['bss_owners'], ev['rows']):
                r = next(r for r in rows if r['id'] == 'raw_bss_obj_seg006')
                r['raw_form'] = bss_link.COMMUNAL_UNIT
                r.pop('object', None)
                if 'object' in r or rows is ev['rows']:
                    r['object'] = None
        self.refused(communal_early, pattern='communal unit')

        def gap(manifest, ev):
            raw(manifest, 'raw_bss_obj_seg006')['start'] += 2
        self.refused(gap, pattern='gap/overlap')

        def stale_evidence(manifest, ev):
            ev['rows'].append({'id': 'raw_bss_extra', 'raw_form': bss_link.OBJECT_BSS, 'object': 'obj_seg011',
                               'start': 1, 'end': 2})
        self.refused(stale_evidence, pattern='without a partition row')

        def partial_claim(manifest, ev):
            row = next(r for r in manifest['bss_owners'] if r['id'] == 'obj_seg001_complete:_BSS')
            nxt = raw(manifest, 'raw_bss_obj_seg005')
            row['end'] += 2
            nxt['start'] += 2
        self.refused(partial_claim, pattern='reviewed placeholder|placeholder exactly')

        def twice(manifest, ev):
            for rows in (manifest['bss_owners'], ev['rows']):
                next(r for r in rows if r['id'] == 'raw_bss_obj_seg006')['object'] = 'obj_seg005'
        self.refused(twice, pattern='two BSS rows')


def _module(mid, code, bss=0, owners=(), host=None, grounded=None):
    return {'id': mid, 'code': list(code), 'code_nonempty': list(code), 'bss': bss, 'bss_align': 'word',
            'owners': list(owners), 'host': host, 'grounded': grounded}


class PlacementAfterRawTests(unittest.TestCase):
    EDATA = 199994
    SEGS = ['S000_TEXT', 'S001_TEXT', 'S007_TEXT', 'S008_TEXT', '_TEXT', 'S027_TEXT']

    def order(self, modules, start, size=16, rid='o:_BSS'):
        row = {'id': rid, 'start': start, 'end': start + size}
        return bss_link.independent_static_order([row], modules, self.SEGS, self.EDATA)[rid]

    def base(self):
        return [_module('m0', ['S000_TEXT'], 12, ['x:_BSS']),
                _module('m1', ['S001_TEXT']), _module('p1', [], 36, host='m1'),
                _module('m7', ['S007_TEXT']), _module('p7', [], 1907, host='m7'),
                _module('rt1', ['_TEXT']), _module('rt2', ['_TEXT']),
                _module('prt', [], 38, host='rt2', grounded=200006 + 36 + 1907 + 1 + 16)]

    def test_raw_placeholder_takes_its_host_position(self):
        modules = self.base() + [_module('m8', ['S008_TEXT'], 16, ['o:_BSS'])]
        result = self.order(modules, 205384 - 290 - 3144)
        self.assertEqual(result['problems'], [])
        self.assertEqual(result['before'], ['m0', 'p1', 'p7'])
        self.assertEqual(result['after'], 1)      # the grounded runtime placeholder: _TEXT lies later
        wrong = self.order(modules, 205383 - 290 - 3144)
        self.assertTrue(any('image-derived order places' in p for p in wrong['problems']))

    def test_grounded_runtime_placeholder_fits_before_a_later_claim(self):
        modules = self.base() + [_module('m8', ['S008_TEXT'], 16, ['y:_BSS']),
                                 _module('m27', ['S027_TEXT'], 48, ['o:_BSS'])]
        at = 200006 + 36 + 1907 + 1 + 16 + 38
        result = self.order(modules, at, 48)
        self.assertEqual(result['problems'], [])
        self.assertEqual(result['before'], ['m0', 'p1', 'p7', 'm8', 'prt'])
        misfit = copy.deepcopy(modules)
        next(m for m in misfit if m['id'] == 'prt')['grounded'] += 2
        result = self.order(misfit, at + 2, 48)
        self.assertTrue(any('do not fit' in p for p in result['problems']))

    def test_ungrounded_shared_host_is_ambiguous(self):
        modules = self.base() + [_module('m27', ['S027_TEXT'], 48, ['o:_BSS'])]
        next(m for m in modules if m['id'] == 'prt')['grounded'] = None
        result = self.order(modules, 200006 + 36 + 1907 + 1 + 38, 48)
        self.assertIn('prt', result['ambiguous'])

    def test_placeholder_follows_host_in_processing_order(self):
        import reallink
        a, b, c = (reallink.Unit(n, 'c', 'S00%d_TEXT' % i, i, i + 1) for i, n in enumerate('abc'))
        p = reallink.Unit('bssraw_a', 'bss', '_BSS', 10, 20)
        p.host = a
        self.assertEqual([u.id for u in reallink.place_bss_placeholders([a, b, c, p])],
                         ['a', 'bssraw_a', 'b', 'c'])

    def test_grounded_member_bss_reads_own_fixup_operands(self):
        class Obj:
            linker_fixups = [{'target_kind': 'segment', 'target': '_BSS', 'segment': '_TEXT', 'loc': 'offset16',
                              'self_relative': False, 'offset': o, 'encoded_addend': e, 'displacement': 0}
                             for o, e in ((2, '0000'), (6, '0400'))]
        image = bytearray(16)
        image[2:4] = (207000 - 178032).to_bytes(2, 'little')
        image[6:8] = (207004 - 178032).to_bytes(2, 'little')
        self.assertEqual(bss_link.grounded_member_bss(Obj(), 0, bytes(image), 178032), (207000, 2))
        image[6:8] = (207006 - 178032).to_bytes(2, 'little')
        self.assertEqual(bss_link.grounded_member_bss(Obj(), 0, bytes(image), 178032)[0], None)


class DiagnosticModelTests(unittest.TestCase):

    def test_communal_model_reproduces_real_link_maps(self):
        import communal_order
        corpus = json.loads((FIXTURES / 'link365_communal_fixtures.json').read_text())
        self.assertEqual(len(corpus['fixtures']), 5)
        for fx in corpus['fixtures']:
            rows = communal_order.order([tuple(c) for c in fx['commons']], [tuple(o) for o in fx['others']])
            observed = {communal_order._key(n): (k, off) for n, k, off in fx['observed']}
            base = {}
            for row in rows:
                kind, off = observed[communal_order._key(row['name'])]
                self.assertEqual(kind, row['kind'], fx['label'])
                base.setdefault(kind, off)
                self.assertEqual(off - base[kind], row['offset'], (fx['label'], row['name']))
            for kind in ('near', 'far'):
                want = [communal_order._key(r['name']) for r in rows if r['kind'] == kind]
                got = [communal_order._key(n) for n, k, off in sorted(fx['observed'], key=lambda t: t[2])
                       if k == kind]
                self.assertEqual(got, want, fx['label'])
        # the thin bss_link wrapper keeps its integ32 interface
        wrapped = bss_link.communal_order([{'name': '_b', 'size': 2, 'encounter': 1}], near_base=207396)
        self.assertEqual(wrapped[0]['address'], 207396)

    def test_file_c_buffers_fall_in_their_target_buckets(self):
        import communal_order
        self.assertEqual([communal_order.bucket(n) for n in ('__bufout', '__bufin', '__buferr')], [43, 102, 233])

    def test_static_model_reproduces_compiled_fixtures(self):
        corpus = json.loads((FIXTURES / 'msc510_static_order_fixtures.json').read_text())
        self.assertEqual(len(corpus['short_fixtures']), 44)
        for fx in corpus['short_fixtures']:
            names = fx['declared']
            offsets = bss_link.static_order(names, {n: 2 for n in names}, {n: 'short' for n in names})
            self.assertEqual(sorted(offsets, key=offsets.get), fx['observed_order'], fx['fixture'])
            self.assertEqual(max(offsets.values()) + 2, fx['bss_length'], fx['fixture'])
        typed = corpus['typed_fixture']
        names = [n for n, _, _ in typed['declared']]
        sizes = {n: s for n, _, s in typed['declared']}
        types = {n: t for n, t, _ in typed['declared']}
        self.assertEqual(bss_link.static_order_residual(names, sizes, types, typed['observed_offsets']), {})

    def test_models_are_diagnostic_only(self):
        importers = []
        for path in sorted((ROOT / 'tools').glob('*.py')):
            text = path.read_text(encoding='utf-8', errors='replace')
            if re.search(r'^\s*(import|from)\s+(communal_order|msc_static_model)\b', text, re.M):
                importers.append(path.name)
        # integ39: the landed commfit name solver is itself a DIAGNOSTIC design aid;
        # no acceptance module imports a model.
        self.assertEqual(importers, ['bss_link.py', 'commfit.py'])
        for gate_module in ('communal_unit.py', 'reallink.py', 'promote.py', 'batch_publish.py', 'build_exact.py'):
            text = (ROOT / 'tools' / gate_module).read_text(encoding='utf-8')
            self.assertNotRegex(text, r'^\s*(import|from)\s+(communal_order|msc_static_model|commfit)')
        source = (ROOT / 'tools/bss_link.py').read_text(encoding='utf-8')
        for gate in ('def check_static', 'def gate', 'def independent_static_order', 'def check_communals'):
            body = source.split(gate, 1)[1].split('\ndef ', 1)[0]
            self.assertNotIn('communal_order(', body)
            self.assertNotIn('static_order(', body.replace('independent_static_order(', ''))


if __name__ == '__main__':
    unittest.main()
